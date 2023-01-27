
## Procedural Terrain Generation
### Overview:
For this procedural terrain generation project, I used a perlin noise like function to generate a random height map. Then in the shader file, based on the different numbers (representing heights), the program assigns a color to that pixel. For example, if the number at a pixel is greater than a certain threshold, then the program will color it white, so that it looks like snow on top of a mountain. Or if the number is smaller, it will be colored green so it looks like part of the mountain. 


### To run this program:
* First pull this project to your local machine.
* Then run: 
 * `cd part1`
 * `python3 build.py`
 * `./project`
* After running these three commands, you will see the project window.

### To interact with this program:
* You can press the 'o' key to make the terrain more detailed. However, there is a limit to how detailed it will look, so after exceeding that limit, the terrain will be set to the least detailed version. 
* You can move your mouse to look around as if you are actually standing on the terrain.
* You can use the arrow keys to move to the left, to the right, to the front, and back as if you are walking on the terrain.

### See the demo: https://www.youtube.com/watch?v=XBfumlxLRew

### Screenshots from the program
<img width="1273" alt="Screen Shot 2022-12-11 at 10 58 33 AM" src="https://user-images.githubusercontent.com/42773608/206914203-f3fb4f9c-049e-4209-a178-99ef7a26075d.png">
<img width="1271" alt="Screen Shot 2022-12-11 at 11 00 06 AM" src="https://user-images.githubusercontent.com/42773608/206914285-89f29b25-eeb1-4a01-8a96-992f0c6cfcd0.png">
<img width="1276" alt="Screen Shot 2022-12-13 at 8 09 31 PM" src="https://user-images.githubusercontent.com/42773608/207480133-0243e67b-7aae-4f83-98cb-2abbcc061201.png">

* Resources that I used:
  * https://www.youtube.com/watch?v=6-0UaeJBumA
  * https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/procedural-patterns-noise-part-1/simple-pattern-examples
