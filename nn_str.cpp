/*
Copyright (c) 2013, K S Ernest Lee
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of K S Ernest Lee nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Timothee Besset BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "../nn.hpp"
#include <stdarg.h>

/*  Use a string formatted with format strings and return new buffer. 
    If there was not enough memory, returns NULL. */
char * s_vprintf (const char *format, va_list argptr)
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
int nstr_send (nn::socket sock, const char *format, ...)
{
    assert (format);
    
    va_list argptr;
    va_start (argptr, format);
    char *string = s_vprintf (format, argptr);
    va_end (argptr);

    int rc = sock.send(string, NN_MSG, 0); 
    assert (rc == sz_msg);
    return rc;
}

/* Get C string. Returns NULL if there is an error. 
   Must call nn_freemsg(buf) after use */
char * nstr_recv (nn::socket sock)
{
    void *buf = NULL;
    const int nbytes = sock.recv(&buf, NN_MSG, 0);
    assert (nbytes >= 0);
    char *string = (char *) malloc (nbytes + 1);
    memcpy (string, &buf, nbytes);
    nn_freemsg (buf);
    return string;
}
