#include "MapReduceFramework.h"
#include <cstdio>
#include <string>
#include <array>
#include <unistd.h>
#include <iostream>
class VString : public V1, public V2, public V3 {
public:
	VString(std::string content) : content(content) { }
	std::string content;

};

class KString : public K1, public K2, public K3 {
    public:
    KString(std::string content) : content(content) { }
	std::string content;
    virtual bool operator<(const K1 &other) const {
		return  content < static_cast<const KString&>(other).content;
	}
    virtual bool operator<(const K2 &other) const {
		return  content < static_cast<const KString&>(other).content;
	}
	virtual bool operator<(const K3 &other) const {
		return content < static_cast<const KString&>(other).content;
    }


};

class KChar : public K2, public K3{
public:
	KChar(char c) : c(c) { }
	virtual bool operator<(const K2 &other) const {
		return c < static_cast<const KChar&>(other).c;
	}
	virtual bool operator<(const K3 &other) const {
		return c < static_cast<const KChar&>(other).c;
	}
	char c;
};

class VCount : public V2, public V3{
public:
	VCount(int count) : count(count) { }
	int count;
};

KString gk = KString("\t");

 
// class TestableClient : public MapReduceClient {
//     virtual void out() const = 0;

// };


class UnChangeClient : public MapReduceClient {
public:
	void map(const K1* key, const V1* value, void* context) const {
        emit2( &gk, (V2 *)( (VString * ) value),  context);

	}
    
	virtual void reduce(const IntermediateVec* pairs, 
		void* context) const {

		for(const IntermediatePair& pair: *pairs) {
		    emit3( (K3 *)  ((KString * ) pair.first), (V3 *) ( (VString *) pair.second), context);
		}
		usleep(150000);
	}
};

class CounterClient : public MapReduceClient {
public:
	void map(const K1* key, const V1* value, void* context) const {
		std::array<int, 256> counts;
		counts.fill(0);
		for(const char& c : static_cast<const VString*>(value)->content) {
			counts[(unsigned char) c]++;
		}

		for (int i = 0; i < 256; ++i) {
			if (counts[i] == 0)
				continue;

			KChar* k2 = new KChar(i);
			VCount* v2 = new VCount(counts[i]);
			usleep(150000);
			emit2(k2, v2, context);
		}
	}

	virtual void reduce(const IntermediateVec* pairs, 
		void* context) const {
		const char c = static_cast<const KChar*>(pairs->at(0).first)->c;
		int count = 0;
		for(const IntermediatePair& pair: *pairs) {
			count += static_cast<const VCount*>(pair.second)->count;
			delete pair.first;
			delete pair.second;
		}
		KChar* k3 = new KChar(c);
		VCount* v3 = new VCount(count);
		usleep(150000);
		emit3(k3, v3, context);
	}
};



void santy_test()
{
    UnChangeClient client;
	InputVec inputVec;
	OutputVec outputVec, outputVec2;
	VString s1("This string is full of characters");
	VString s2("Multithreading is awesome");
	VString s3("race conditions are bad");
	inputVec.push_back({nullptr, &s1});
	inputVec.push_back({nullptr, &s2});
	inputVec.push_back({nullptr, &s3});
	JobState state;
    JobState last_state={UNDEFINED_STAGE,0};
	JobHandle job = startMapReduceJob(client, inputVec, outputVec, 4);
	startMapReduceJob(client, inputVec, outputVec2 , 4);
	getJobState(job, &state);
    
	while (state.stage != REDUCE_STAGE || state.percentage != 100.0)
	{
        if (last_state.stage != state.stage || last_state.percentage != state.percentage){
            printf("stage %d, %f%% \n", 
			state.stage, state.percentage);
        }
		usleep(100000);
        last_state = state;
		getJobState(job, &state);
	}
	printf("stage %d, %f%% \n", 
			state.stage, state.percentage);
	printf("[v][ test-1] Done!\n");
	
	closeJobHandle(job);
	
	for (OutputPair& pair: outputVec) {
		std::string c = ((const KString *)pair.first)->content;
        std::string q = ((const VString*)pair.second)->content;
        std::cout << c << " --- " << q << std::endl; 

        // printf("The character %c appeared %d time%s\n", 
		// 	c, count, count > 1 ? "s" : "");
		// delete pair.first;
		// delete pair.second;
	}
} 

void enumerate_test() 
{
    int input_number, threads_num; std::cin >> input_number >> threads_num;
    CounterClient client;
	InputVec inputVec;
	OutputVec outputVec;


    for ( int i = 0 ; i  < input_number; i++ ){
        std::string inp; std::cin >> inp;
        inputVec.push_back({nullptr, new VString(inp)});
    }

	JobState state;
    JobState last_state={UNDEFINED_STAGE,0};
	JobHandle job = startMapReduceJob(client, inputVec, outputVec, threads_num);
	getJobState(job, &state);
    
	while (state.stage != REDUCE_STAGE || state.percentage != 100.0)
	{
        if (last_state.stage != state.stage || last_state.percentage != state.percentage){
            printf("stage %d, %f%% \n", 
			state.stage, state.percentage);
        }
		usleep(100000);
        last_state = state;
		getJobState(job, &state);
	}
	printf("stage %d, %f%% \n", 
			state.stage, state.percentage);

	printf("[v][Done] enumerate_test!\n");
	
	closeJobHandle(job);
	
	for (OutputPair& pair: outputVec) {
        char c = ((const KChar *)pair.first)->c;
        int count = ((const VCount * )pair.second)->count;

        printf("The character %c appeared %d time%s\n", 
			c, count, count > 1 ? "s" : "");
	}
}

int main(int argc, char** argv)
{

    int client_method; std::cin >> client_method;
    void (* ptrs [])() = { &santy_test, &enumerate_test } ;
    ptrs[client_method]();

	
	
	return 0;
}
