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

13/09/2020:
1: Fixed moon lighting (sent shader int value, when it need float. Should have tested, instead of going to bed).
2: changed some stepsize params.

14/09/2020
1: testing averaging the time_difference calc to fix the cloud jump at midnight.
2: testing for time_up_lots etc. Use avg unless time_up_lots.
3: Fully fixed cloud jump, and can still use "shift + L/K" for time advance. 
4: tidy up a bit, and comments for the added code.

16/09/2020
1: Added a standard bicubic sample pattern for testing (FPS/Quality) catmull rom is still the default.
   sample_pattern	= 0 catmull.
					= 1 std bicubic.
   May add more sample patterns.

todo: 
change moon calc to midday to stop jump in glow.
Add widget interface??