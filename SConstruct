# sudo apt-get install scons;
# scons rso;
# sudo scons install;

Program('rso', ['src/rso.c', 'src/rso_io.c'],
	LIBS='libsndfile', LIBPATH='/usr/lib/i386-linux-gnu/')

Command('install', None, 'cp rso /usr/bin')