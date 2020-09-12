# Enhanced-Cloudscapes
A freeware volumetric cloud replacement add-on for X-Plane 11

Personal build.

Changes:

3/9/2020
1: changed texture creation to nearest filter, to help remove sparkles on edges.
2: Fixed plugin depthbuffer creation (no long a blank texture). Doesn't look to fix the issues with depth.
3: Added a bicubic filter in post processing shader swicthable with the bicubic_sampling dataref.

4/9/2020
1: Improvements to nightlighting (render shader). Added moon tracking, and moon_gain and moon_glow to sim objects, render_prog and shader. 

5/9/2020
1: Small changes to moon_gain/glow.

11/09/2020
1: small changes again to nightlighting.
2: changed density of cirrus clouds. (personal pref).

12/09/2020
1: Added moon_phase calc for full moon tracking, and surface percentage figure.
2: Changes to render frag_shader.