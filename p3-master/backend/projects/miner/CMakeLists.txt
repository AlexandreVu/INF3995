set (zmqMiner
    minerCommunication/MinerCommunication.cpp 
    minerCommunication/MinerCommunication.h
)

set (blockchain
    blockchain/Block.h
    blockchain/Block.cpp
    blockchain/Blockchain.h
    blockchain/Blockchain.cpp
)

add_executable(miner main.cpp ${zmqMiner} ${blockchain})
target_compile_options(miner PRIVATE ${WARNING_FLAGS} )
target_link_libraries(miner PRIVATE common crypto)