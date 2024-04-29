all:
	gcc -o memory_sim main.c memory_manager.c thread_handler.c utils.c -lpthread
