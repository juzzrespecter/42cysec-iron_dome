docker build -t iron_dome_image .
docker run -it -v $PWD/code:/code -v $PWD/files:/files --name iron_dome iron_dome_image  