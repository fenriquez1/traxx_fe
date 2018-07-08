CPP = g++

CFLAGS = -std=c++11 -O3 -g3 -Wall -c -fmessage-length=0

LFLAGS =

RM = rm -rf

CPP_SRCS += \
src/CoordinateConverter.cpp

OBJS += \
./CoordinateConverter.o

CPP_DEPS += \
src/CoordinateConverter.d

LIBS = -lpistache -lpthread

LIBS_DIR = /usr/local/lib/

all: CoordinateConverter

CoordinateConverter: $(OBJS)
	$(CPP) $(LFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): $(CPP_SRCS)
	$(CPP) $(CFLAGS) -MMD -MP -MF "$(@:%.o=%.d)" -MT"$(@)" $(CPP_SRCS)

clean:
	-$(RM) $(OBJS) CoordinateConverter*

.PHONY: all clean dependents
