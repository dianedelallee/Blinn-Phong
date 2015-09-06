# Blinn-Phong

Made in 2013 with E. Bonnefoy

Building
------
Open a terminal
move to your directory
and write these lines :
```
make config-release
./bin/release/projet/syntheseImage
```

If some mistakes happens, it s due to our model 3D loader (.ply).
To solve them, open the file main.cpp, go to lines 278 (until 312) and change the path of the model (.ply).

Commands
------
* F1 : show global illumination
* F2 : show only colors (materail component)
* F3 : show normals
* F4 : show depth buffer
* F5 : show positions
* F6 : show the scene with multi-pass ilumination
* F8 : show the scene with SSAO and bloom effect
* F9 : show the scene with SSAP and gloabl illumination

If you want to show SSAP effect on a different scene<br/>
* Inser : Dragon with SSAO
* Suppr : Dragon without SSAO

If you want to move in the scene, you have to use the mouse :<br/>
* left clic : rotation of the camera
* middle clic : change repere 
* right clic : zoom in/out

* Help menu : Caps Lock/Shift

Images
------
<img src="http://diane-delallee.fr/assets/images/dragon.png" width="49%">


License
------
![alt cc](https://licensebuttons.net/l/by/3.0/88x31.png)
This work is licensed under a [Creative Commons Attribution 4.0 International License] (http://creativecommons.org/licenses/by/4.0/)
