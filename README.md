# Reflexion

Los tilemaps estan basados en la lava que se encuentra debajo del oceano, por lo tanto se buscan hacer obstaculos con estos.
El sistema de tilemaps permite construir de manera modular el entorno oceánico para la ballena, por lo tanto ayuda a generar su ambientacion.
Para lograrlo el mapa del nivel se representa en el código como una matriz (un arreglo bidimensional). Cada número o valor en la matriz representa un tipo de tile específico en la imagen. Se itera a través de cada posición de la matriz para renderizar el tile correspondiente. Cada número en la matriz indica qué parte de la imagen del tilemap se debe mostrar.

https://github.com/user-attachments/assets/aebbf10d-1d2e-47e7-9f43-53b1431d32a0
