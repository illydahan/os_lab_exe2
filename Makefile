# Makefile for the banking program

CC = g++

CXXFLAGS = -g -Wall -std=c++11  -Werror -pedantic-errors -DNDEBUG -pthread -lpthread

LDFLAGS = -pthread -lpthread

CCLINK = $(CC)

RM = rm -rf

OBJS = RWLock.o ATM.o Logger.o Bank.o

# creating executable

Bank: $(OBJS)
		$(CCLINK) $(LDFLAGS) -o Bank $(OBJS)

# creating object files


RWLock.o: RWLock.cpp RWLock.h
ATM.o: ATM.cpp ATM.h
Logger.o: Logger.cpp Logger.h
Bank.o: Bank.cpp

clean:
	$(RM) $(TARGET) *.o *~ "#"* core.* Bank
