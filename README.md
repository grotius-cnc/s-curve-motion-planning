# S-curve-motion-planning.

[![grotius-cnc - s-curve-motion-planning](https://img.shields.io/static/v1?label=grotius-cnc&message=s-curve-motion-planning&color=blue&logo=github)](https://github.com/grotius-cnc/s-curve-motion-planning "Go to GitHub repo")
[![stars - s-curve-motion-planning](https://img.shields.io/github/stars/grotius-cnc/s-curve-motion-planning?style=social)](https://github.com/grotius-cnc/s-curve-motion-planning)
[![forks - s-curve-motion-planning](https://img.shields.io/github/forks/grotius-cnc/s-curve-motion-planning?style=social)](https://github.com/grotius-cnc/s-curve-motion-planning)

[![GitHub release](https://img.shields.io/github/release/grotius-cnc/s-curve-motion-planning?include_prereleases=&sort=semver&color=blue)](https://github.com/grotius-cnc/s-curve-motion-planning/releases/)
[![License](https://img.shields.io/badge/License-MIT-blue)](#license)

The purpose of this material is to impart an understanding of polynomial transitions for a trajectory generator. The ideal constant jerk S-curve
(jerk is the derivate of acceleration and a measure of impact) can be represented by a second-order polynomial in velocity (3rd in position). 
Its shape is governed by the motion conditions at the start and end of the transition.

An S-curve with an intermediate constant acceleration (lineair portion) is often used to reduce the time to make large speed changes. The jerk can be
used to determine how much of the rise or fall period can be made under constant acceleration.

![scurve_example](https://user-images.githubusercontent.com/44880102/147381940-9394686b-a1be-40a1-8397-6f017c47851b.jpg)

Implementation:

- Velocity up s-curve [acc period].
- Velocity down s-curve [dcc period].
- Acceleration begin value.
- Acceleration end value.
- Velocity begin.
- Velocity end.
- Max acceleration.
- Lineair stage.
- Scientific papers included.

Build in Logic:

- If maximum velocity can not be reached for a motion, curves are sampled to fit.
- Debug information when input values are out of scope.

Performance:

    Time taken to calculate a motion nanoseconds: ~1500 ~4051 

Graphic implementation:

    ~/gui_project/motion/
    
![scurvemotion](https://user-images.githubusercontent.com/44880102/147080009-f6e50645-2be6-46e6-a253-6fbf8488c1de.jpg)

To use the opencascade graphics along with the gui project, follow these instructions : https://github.com/grotius-cnc/oce/releases/tag/1.0.1

## Summary
Constant jerk S-curves are used to transition robot moves. The S-curve is used to eliminate discontinuities in acceleration that are detrimental
to robot performance.

## License

Released under [MIT](/LICENSE) by [@grotius-cnc](https://github.com/grotius-cnc).
