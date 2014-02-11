/**
 * @author Daniel Escoz Solana
 */
#include <memory>
#include "ImageScene.hpp"

int main (int argc, char** argv) {
    std::unique_ptr<Scene> scene (new ImageScene());
    return scene->run();
}