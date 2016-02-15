all :
	cd gifitgetinput; gcc gifitgetinput.c xutils.c writer.c -o gifitgetinput -lX11 -lpthread
