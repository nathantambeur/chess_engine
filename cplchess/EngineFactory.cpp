#include "EngineFactory.hpp"

std::unique_ptr<Engine> EngineFactory::createEngine() {
    return std::unique_ptr<Engine>(new Search());  
}
