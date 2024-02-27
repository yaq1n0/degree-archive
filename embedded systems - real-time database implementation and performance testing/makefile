CPPFLAGS += -I include -W -Wall -Wno-unused-parameter -g -std=c++17
LDFLAGS += -pthread

# Flags for checked build
#CPPFLAGS += -fsanitize=undefined -fsanitize=thread
# Cannot have both address and thread sanitisers at the same time
#CPPFLAGS += -fsanitize=address 

# Flags for release build
CPPFLAGS += -O3 -DNDEBUG=1

ALL_HEADERS := $(wildcard include/*.hpp include/*/*.hpp)

bin/% : src/%.cpp $(ALL_HEADERS)
	mkdir -p bin
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

all_distrib : \
	bin/benchmark_real_time_database \
	bin/mini_system_queue_1task      \
	bin/mini_system_queue_2task      \
	bin/mini_system_queue_3task      \
	bin/mini_system_threads_3task	 \
	bin/yes
	
all_deliverables : all_distrib \
	include/databases/LockedRealTimeDatabase.hpp \
	include/databases/EfficientRealTimeDatabase.hpp \
	graphs/1.3-efficient_db_access_scaling.pdf \
	include/databases/ScalableRealTimeDatabase.hpp \
	graphs/1.5-scalable_db_update_scaling.pdf \
	include/databases/ScalableRealTimeDatabase.hpp \
	graphs/1.6-scalable_db_thread_scaling.pdf \
	graphs/2.1-queue_task1_quiet.pdf \
	graphs/2.2-queue_task1_high_jitter.pdf \
	graphs/2.3-queue_volume_versus_latency.pdf \
	src/mini_system_threads_3task.cpp \
	graphs/2.5-queue_volume_versus_latency.pdf 
