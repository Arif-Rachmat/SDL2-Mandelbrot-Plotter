# SDL2-Mandelbrot-Plotter


[![Download][download-shield]][download-url]
[![SDL][sdl-shield]][sdl-url]
[![License][license-shield]][license-url]

My version of a Mandelbrot set plotter using SDL2 Library 



<details open>
    <summary>Table Of Content</summary>
    <ol>
        <li><a href="#overview">Overview</a></li>
        <li><a href="#getting-started">Getting Started</a></li>
        <li>
        <details>
            <summary><a href="#usage">Usage</a></summary>
            <ul>
            <li><a href="#control">Control</a></li>
            <li><a href="#terminal">Terminal</a></li>
            </ul>
        </details>
        </li>
        <li><a href="#license">License</a></li>
    </ol>
</details>
<br>
<br>
<br>

# Overview
The Mandelbrot set is a set of complex number on the complex plane that will not deviate when it is iterated multiple time with the formula $Z_{n+1}=Z_n^2+C$ .

This program plot the mandelbrot set by mapping out each pixel according to the coordinate of the complex plane and then iterate it multiple time with the formula mentioned above until it exceed a certain value. Each pixel is then colored according to the number of iteration it reaches, unless it reaches the maximum iteration allowed in this case the pixel is then colored black.

Using this program you can 

<p align="right"><a href="#sdl2-mandelbrot-plotter">[Back to Top]</a></p>




# Getting Started
Download the latest version from [release](). Extract the binaries and dll to a single directory, or extract the whole folder.

When you open the program you
<img src="image/ui.png">

<p align="right"><a href="#sdl2-mandelbrot-plotter">[Back to Top]</a></p>




# Usage
This section will show you how to use the program and multiple way of using it to explore the mandelbrot set.
<br>
<br>

>## Control

|                  Key                  |           Function                   |
|---------------------------------------|--------------------------------------|
|`Left click`                           | Zoom in to the cursor location       |
|`Right click`                          | Zoom out from the cursor location    |
|<kbd>E</kbd>                           | Zoom in to the locked coordinate     |
|<kbd>Q</kbd>                           | Zoom out from the locked coordinate  |
|<kbd>U</kbd>                           | Toggle info UI                       |
|<kbd>Z</kbd>                           | Toggle AutoZoom                      |
|`Mousewheel Up`                        | Increase Max iteration               |
|`Mousewheel Down`                      | Reduce Max iteration                 |
|<kbd>LCtrl</kbd> + `Mousewheel Up`     | Increase iteration stepping value    |
|<kbd>LCtrl</kbd> + `Mousewheel Down`   | Reduce iteration stepping value      |
|<kbd>LShift</kbd> + `Mousewheel Up`    | Increase Zooming multiplier value    |
|<kbd>LShift</kbd> + `Mousewheel Down`  | Reduce Zooming multiplier value      |

<br>
<br>
<br>

>## Terminal
Openning the program directly by clicking it will run the program with the default configuration, while running the program from the terminal will provide more option and configuration.
```powershell
mandelbrot.exe [argument]
```
*All argument can be passes together in any order

<br>
<br>

Some of the available arguments are :

<br>

- ### Resolution
This argument will set the window resolution. Setting this to a high value will slows down the rendering time. The default value is `800 x 800` .
```powershell
mandelbrot.exe -r [width] [height]
```
example of setting the resolution to 1920 x 1080 :
```powershell
mandelbrot.exe -r 1920 1080
```
<br>

- ### Zoom Coordinate
This argument will set the starting [zoom coordinate](). Example of the zooming coordinate can be found in [here](http://www.cuug.ab.ca/dewara/mandelbrot/images.html). The default starting coordinate is `X = 0.281717921930775` & `Y = 0.5771052841488505`
```powershell
mandelbrot.exe -c [X Coordinate] [Y Coordinate]
```
example of setting the starting zoom coordinate to `X = 0.840719` &
`Y = 0.22442`
```Powershell
mandelbrot.exe -c 0.840719 0.22442
```
<br>

- ### Color Palette

<p align="right"><a href="#sdl2-mandelbrot-plotter">[Back to Top]</a></p>




# License
Distributed under the GNU General Public V3.0 License. See [`License`]() for more information.

<p align="right"><a href="#sdl2-mandelbrot-plotter">[Back to Top]</a></p>




<!-- Link for badge and others -->
[download-shield]: https://img.shields.io/badge/Download-v1.0.0-brigthgreen
[download-url]: -
[sdl-shield]:  https://img.shields.io/badge/SDL-v2.0.20-brigthgreen
[sdl-url]: https://www.libsdl.org/
[license-shield]: https://img.shields.io/badge/License-GNU-blue
[license-url]: -