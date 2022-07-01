include Make.define

PROGS = cli serv	#两个目标程序
SUB_DIR = app		#一个子目录
SUB_DIR_CLEAN := $(SUB_DIR:%=%_clean)

.PHONY: subdirs $(SUB_DIR)	#表示subdirs $(SUB_DIR)不是代表这个makefile中的一个文件名称
#表示这个makefile不会对相同目录下名为subdirs $(SUB_DIR)的文件做任何操作。

all:${PROGS} $(SUB_DIR)	#最根部的依赖

cli: client.o
	${CC} ${CFLAGS} -o $@ client.o ${LIBS}

serv: server.o
	${CC} ${CFLAGS} -o $@ server.o ${LIBS}

clean:$(SUB_DIR_CLEAN) 
	rm -f ${PROGS} ${CLEANFILES}


$(SUB_DIR):
	@+make -C $@	# "-C"切换到指定目录再执行 make 过程,makefile 在这个指定目录里面
#前面加上at符号@,表示在make执行时候,输出的信息中,不要显示此行命令。

$(SUB_DIR_CLEAN):
	@+make clean -C $(@:%_clean=%)
