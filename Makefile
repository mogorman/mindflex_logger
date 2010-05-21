all: mindflex_logger

mindflex_logger:
	gcc -o mindflex_logger mindflex_logger.c

install: all
	chmod +x mindflex_loop.sh
	chmod +x mindflex.init
	cp mindflex_logger /usr/local/bin/
	cp mindflex_loop.sh /usr/local/bin/
	cp mindflex.init /etc/init.d/mindflex
clean:
	rm -rf mindflex_logger

