# es_core: an experimental framework for low latency, high fps multiplayer games.

Written by Timothee Besset.

es_core: an experimental framework for low latency, high fps multiplayer games.
https://github.com/TTimo/es_core

The current state of competitive, skilled FPS games is a topic that comes up often in discussions among my game industry friends, with players and with anyone involved with eSports.

There is lament that no FPS title has been able to grow a big enough audience and pull enough weight around to be represented on the major circuits like MLG or ESL recently. There are a lot of ins and outs to that discussion, but one aspect that is of particular interest to me is the projects that are at various stages of development, trying to crack that nut and make it big for an eSport FPS title again.

I need to mention a few of those to ground the discussion a bit. QuakeLive is the old guard, it's still around and kicking, surprisingly resilient etc. Then there are newer, more active titles, like CS:GO and Shootmania. Further down on the horizon there's rumored projects like James Harding's Reborn.

This got me thinking about how I would approach this, and what my ideal engine framework would look like. We are talking PC-centric, mouse and keyboard gaming here. High end GPU, multicore CPU, 120 Hz display and low latency, high precision input.

Renderer eye candy should never get in the way of a smooth game simulation. A sudden dip in renderer fps should never mean that you are going to miss that jump, or that your mouse handling is going to turn you around in a slightly different way than usual.

The technology foundation for a project like this is extremely important. Unity and UDK are amazing engines, but they are general purpose in nature, and they made compromises to that end which make them ill-suited.

So I decided to put some code together to explore the subject. There's just enough stuff now that I feel it's worth putting it out to get some feedback. I've named this es_core, you can find the project on github.

And now I'll just ramble on about what's in the code. I picked Ogre as the renderer, it's a simple API, great cross platform support, and for the most part it only cares about doing rendering. I went to SDL2 for window creation, rendering context setup and input management.

The input is sampled in a dedicated thread, and input state is made available to the game thread or the render thread with a reply/request system. Polling for input on a dedicated thread makes this independent from game logic or renderer timings, which is a big step towards responsive and consistent control.

The threads communicate with each other through message passing only. That system is built on ZeroMQ, a design for which Pieter Hintjens makes a very convincing argument in this article. There is more work in that area: building an efficient binary protocol instead of the current text protocol (I am considering Cap'n Proto), and optimizing the code to use zero-copy message transfers, removing all memory heap operations.

Now to the game and render threads. The game logic runs at a fixed tick (60Hz atm) and updates an internal game state. It produces a new render state with each tick, which is forwarded to the render thread. The renderer runs at the display refresh rate and interpolates between the two most recent render states. There is a great piece by Glenn Fiedler about this approach.

I wanted a small demo to show those pieces working together, but I didn't want to put in any kind of game design, by fear that it would distract from the subject matter. So all you get for now is a head bouncing about that can be spun around with mouse drags. For the most part, writing this was a refreshing exercise on Quaternion math. The demo shows how input can be used by the game thread for general logic and directly by the render thread for smoother, low latency control.

The next big step will be the addition of networked multiplayer. There will probably be another big post like this when that comes together. My current line of thought is that the architecture of the game code should be driven by the needs of the network system, and not the other way around.

This work is a fun side project for me. I'm only giving it a few hours here and there as time and energy permits. It's a great feeling to use all this technology and stand on the shoulders of giants to make new things.