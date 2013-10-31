/*
    Copyright (c) 2013 250bpm s.r.o.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom
    the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#ifndef NN_HPP_INCLUDED
#define NN_HPP_INCLUDED

#include <nanomsg/nn.h>

#include <cassert>
#include <cstring>
#include <algorithm>
#include <exception>
#include <stdarg.h>

#if defined __GNUC__
#define nn_slow(x) __builtin_expect ((x), 0)
#else
#define nn_slow(x) (x)
#endif

namespace nn
{

    class exception : public std::exception
    {
    public:

        exception () : err (nn_errno ()) {}

        virtual const char *what () const throw ()
        {
            return nn_strerror (err);
        }

        int num () const
        {
            return err;
        }

    private:

        int err;
    };

    inline const char *symbol (int i, int *value)
    {
        return nn_symbol (i, value);
    }

    inline void *allocmsg (size_t size, int type)
    {
        void *msg = nn_allocmsg (size, type);
        if (nn_slow (!msg))
            throw nn::exception ();
        return msg;
    }

    inline int freemsg (void *msg)
    {
        int rc = nn_freemsg (msg);
        if (nn_slow (rc != 0))
            throw nn::exception ();
        return rc;
    }

    class socket
    {
    public:

        inline socket (int domain, int protocol)
        {
            s = nn_socket (domain, protocol);
            if (nn_slow (s < 0))
                throw nn::exception ();
        }

        inline ~socket ()
        {
            int rc = nn_close (s);
            assert (rc == 0);
        }

        inline void setsockopt (int level, int option, const void *optval,
            size_t optvallen)
        {
            int rc = nn_setsockopt (s, level, option, optval, optvallen);
            if (nn_slow (rc != 0))
                throw nn::exception ();
        }

        inline void getsockopt (int level, int option, void *optval,
            size_t *optvallen)
        {
            int rc = nn_getsockopt (s, level, option, optval, optvallen);
            if (nn_slow (rc != 0))
                throw nn::exception ();
        }

        inline int bind (const char *addr)
        {
            int rc = nn_bind (s, addr);
            if (nn_slow (rc < 0))
                throw nn::exception ();
            return rc;
        }

        inline int connect (const char *addr)
        {
            int rc = nn_connect (s, addr);
            if (nn_slow (rc < 0))
                throw nn::exception ();
            return rc;
        }

        inline void shutdown (int how)
        {
            int rc = nn_shutdown (s, how);
            if (nn_slow (rc != 0))
                throw nn::exception ();
        }

        inline int send (const void *buf, size_t len, int flags)
        {
            int rc = nn_send (s, buf, len, flags);
            if (nn_slow (rc < 0)) {
                if (nn_slow (nn_errno () != EAGAIN))
                    throw nn::exception ();
                return -1;
            }
            return rc;
        }

        inline int recv (void *buf, size_t len, int flags)
        {
            int rc = nn_recv (s, buf, len, flags);
            if (nn_slow (rc < 0)) {
                if (nn_slow (nn_errno () != EAGAIN))
                    throw nn::exception ();
                return -1;
            }
            return rc;
        }

        inline int sendmsg (const struct nn_msghdr *msghdr, int flags)
        {
            int rc = nn_sendmsg (s, msghdr, flags);
            if (nn_slow (rc < 0)) {
                if (nn_slow (nn_errno () != EAGAIN))
                    throw nn::exception ();
                return -1;
            }
            return rc;
        }

        inline int recvmsg (struct nn_msghdr *msghdr, int flags)
        {
            int rc = nn_recvmsg (s, msghdr, flags);
            if (nn_slow (rc < 0)) {
                if (nn_slow (nn_errno () != EAGAIN))
                    throw nn::exception ();
                return -1;
            }
            return rc;
        }

		/*  Use a string formatted with format strings and return new buffer. 
			If there was not enough memory, returns NULL. */
		inline char * s_vprintf (const char *format, va_list argptr)
		{
			int size = 256;
			char * string = (char *) malloc (size);
			if(!string) {
			  return NULL;
			}
			int required = vsnprintf (string, size, format, argptr); 
		#if defined (__WINDOWS__)
			if (required < 0 || required >= size)
				required = _vscprintf (format, argptr);
		#endif
			//  If the string does not fit, allocate a larger buffer for it.
			if (required >= size) {
				size = required + 1;

				char * temp = (char *) realloc (string, size);
				if (!temp) {
					free(string);
					return NULL;
				}
				string = temp;
        
				vsnprintf (string, size, format, argptr);
			}
			return string;
		}

		/* Send C string. Returns 0 if successful, -1 if there was an failure. */
		inline int nstr_send (const char *format, ...)
		{
			assert (format);
    
			va_list argptr;
			va_start (argptr, format);
			char *string = s_vprintf (format, argptr);
			va_end (argptr);

			int rc = this->send(string, NN_MSG, 0); 
			assert (rc == sz_msg);
			return rc;
		}

		/* Get C string. Returns NULL if there is an error. 
		   Must call nn_freemsg(buf) after use */
		inline char * nstr_recv ()
		{
			void *buf = NULL;
			const int nbytes = this->recv(&buf, NN_MSG, 0);
			assert (nbytes >= 0);
			char *string = (char *) malloc (nbytes + 1);
			memcpy (string, &buf, nbytes);
			nn_freemsg (buf);
			return string;
		}

    private:

        int s;

        /*  Prevent making copies of the socket by accident. */
        socket (const socket&);
        void operator = (const socket&);
    };

    inline void term ()
    {
        nn_term ();
    }

}

#undef nn_slow

#endif


