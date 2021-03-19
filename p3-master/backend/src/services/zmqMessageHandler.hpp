#include <zmq.hpp>
#include <string>
#include <json/json.h>


#define within(num) (int) ((float)((num) * random ()) / (RAND_MAX + 1.0))

using namespace std;
using namespace zmq;
using namespace Json;

inline static bool receiveZMQ(socket_t& socket, string& output, int flags = 0)
{
	message_t message;
	bool isReceived = socket.recv(&message, flags);

	if (isReceived) {
		output = string(static_cast<char*>(message.data()), message.size());
	}
	
	return isReceived;
}

inline static bool sendZMQ(socket_t& socket, const string& output, int flags = 0) {
    message_t message(output.size());
    memcpy (message.data(), output.data(), output.size());

    return socket.send(message, flags);
}

inline static bool sendZMQ(socket_t& socket, const char* output, int flags = 0) {
    return sendZMQ(socket, (string)output, flags);
}

inline static bool sendEmptyZMQ(socket_t& socket){
    message_t message(0);
    return socket.send(message, ZMQ_SNDMORE);
}

inline static bool sendmoreZMQ(socket_t& socket, const string & outupt) {
    return sendZMQ(socket, outupt, ZMQ_SNDMORE);
}


inline static bool receiveZMQ(socket_t& socket, Value& jsonMessage){
    string jsonData;
    bool isReceived = receiveZMQ(socket,jsonData);

    if(isReceived){
        Reader reader;
        reader.parse(jsonData, jsonMessage);
    }

    return isReceived;
}

inline static bool sendZMQ(socket_t & socket, int message, int flags = 0){
    string parsedMessage = to_string(message);

    return sendZMQ(socket, parsedMessage, flags);
}

inline static bool receiveZMQ(socket_t & socket, int& message, int flags = 0){
    string messageData;

    bool isReceived = receiveZMQ(socket, messageData, flags);
    if(isReceived){
        try{
            message = stoi(messageData);
        } catch(...){
            return false;
        }
    }

    return isReceived;
}

inline static bool sendZMQ(socket_t& socket, const Value& jsonMessage, int flag = 0){
    FastWriter fastWriter;
    string jsonData;

    jsonData = fastWriter.write(jsonMessage);
    if(jsonData.compare("null") == 0){
        return false;
    }
    
    return sendZMQ(socket, jsonData, flag);
}

inline static bool sendmoreJsonZMQ(socket_t& socket, const Value& jsonMessage){
    return sendZMQ(socket, jsonMessage, ZMQ_SNDMORE);
}

inline void setIdZMQ (socket_t& socket, const string& id){
    socket.setsockopt(ZMQ_IDENTITY, id.c_str(), id.length());
}

// modified from : https://github.com/booksbyus/zguide/blob/master/examples/C%2B%2B/zhelpers.hpp
inline string setIdZMQ(socket_t& socket){
    stringstream stream;
    stream << hex << std::uppercase
       << setw(4) << setfill('0') << within (0x10000) << "-"
       << setw(4) << setfill('0') << within (0x10000);
    socket.setsockopt(ZMQ_IDENTITY, stream.str().c_str(), stream.str().length());
    
    return stream.str();
}
