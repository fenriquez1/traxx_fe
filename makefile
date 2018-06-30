CPP = g++

CFLAGS = -std=c++11 -O3 -g3 -Wall -c -fmessage-length=0

LFLAGS =

RM = rm -rf

CPP_SRCS += \
src/CoordinateConverter.cpp

OBJS += \
./CoordinateConverter.o

all: CoordinateConverter

ObjectScope: $(OBJS)
	$(CPP) $(LFLAGS) -o $@ $(OBJS)

$(OBJS): $(CPP_SRCS)
	$(CPP) $(CFLAGS) -MT"$(@)" $(CPP_SRCS)

clean:
	-$(RM) $(OBJS) CoordinateConverter

.PHONY: all clean dependents
