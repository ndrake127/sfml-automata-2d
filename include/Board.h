#pragma once

#include <SFML/Graphics.hpp>
#include <thread>
#include <future>
#include <iostream>
#include <string>
#include <random>
#include <time.h>
#include <algorithm>

class Board {
public:
    Board(unsigned windowWidth, unsigned windowHeight, unsigned scale, unsigned drawLimit, unsigned updateLimit, unsigned threadCount, unsigned seed, bool outputPNG) {
        mSeed = (seed != 0) ? seed : time(NULL);
        srand(mSeed);

        std::cout << "Seed: " << mSeed << '\n';
        
        mWindowWidth = windowWidth;
        mWindowHeight = windowHeight;
        mInverseBoardScale = scale;
        mDrawFPSLimit = drawLimit;
        mUpdateFPSLimit = updateLimit;
        mUpdateThreadCount = threadCount;
        mThreadHeights.resize(mUpdateThreadCount + 1, 0);
        mOutputPNG = outputPNG;
        
        if(mInverseBoardScale) {
            mBoardWidth = mWindowWidth / mInverseBoardScale;
            mBoardHeight = mWindowHeight / mInverseBoardScale;
        }

        mWindow.create(sf::VideoMode(mWindowWidth, mWindowHeight), mWindowTitle);
        if(mDrawFPSLimit) mWindow.setFramerateLimit(mDrawFPSLimit);
        initboard();

        for(int i = 1; i < mUpdateThreadCount + 1; i++) {
            mThreadHeights[i] = (((float) i / mUpdateThreadCount) * mBoardHeight);
        }

        std::future<void> UpdateThread = std::async(std::launch::async, [this](){ UpdateLoop(); });

        DrawLoop();
    }

    void DrawLoop() {
        while (mWindow.isOpen())
        {
            sf::Event event;
            while (mWindow.pollEvent(event))
            {
                if (event.type == sf::Event::Closed) {
                    mWindow.close();
                    std::cout << "Average FPS: " << mMaxFPS / mFrames << '\n';
                    exit(1);
                }

                if(event.type == sf::Event::KeyPressed) {
                    switch(event.key.code) {
                        case sf::Keyboard::Escape:
                            mWindow.close();
                            std::cout << "Average FPS: " << mMaxFPS / mFrames << '\n';
                            exit(1);
                            break;

                        case sf::Keyboard::Space:
                            mPause = true;
                            sf::sleep(sf::milliseconds(50));
                            initboard();
                            break;
                    }
                    
                }

                if(event.type == sf::Event::KeyReleased) {
                    switch(event.key.code) {
                        case sf::Keyboard::Space:
                            mPause = false;
                            break;
                    }
                }
            }

            mBoardTexture.loadFromImage(mBoard[mCurrentBoard]);
            mBoardSprite.setTexture(mBoardTexture, true);
            mBoardSprite.setScale(mWindowWidth / mBoardWidth, mWindowHeight / mBoardHeight);

            mWindow.clear();

            mWindow.draw(mBoardSprite);

            mWindow.display();
        }
    }

    void UpdateLoop() {
        while(true) {           
            while(mPause) { continue; }

            sf::Clock clock;
            
            std::future<bool> workers[mUpdateThreadCount];
    
            for(int i = 0; i < mUpdateThreadCount; i++) {
                workers[i] = std::async(std::launch::async, [this](unsigned x, unsigned w, unsigned y, unsigned h) { return update(x, w, y, h); }, 0, mBoardWidth, mThreadHeights[i], mThreadHeights[i+1]);
            }

            for(int i = 0; i < mUpdateThreadCount; i++) {              // Poll threads until all have returned, then restart
                if(workers[i].valid() == false) {
                    i = -1;
                } else {
                    workers[i].get();
                }
            }

            mMaxFPS += 1.0f / clock.getElapsedTime().asSeconds();
            mFrames++;

            if(mOutputPNG) mBoard[mCurrentBoard].saveToFile(std::string("png/" + std::to_string(mFrames) + ".png"));

            while(mUpdateFPSLimit && 1.0f / clock.getElapsedTime().asSeconds() >= mUpdateFPSLimit);

            mCurrentBoard = !mCurrentBoard;
        }
    }

    void initboard() {
        mSeed = rand();
        
        std::cout << "\n\n\nGenerating a new board!\nSeed:" << mSeed << '\n';

        std::default_random_engine generator(mSeed);
        std::uniform_int_distribution<int> distribution(0, mInverseSpawnFrequency);

        mBoard[0].create(mBoardWidth, mBoardHeight, mColorDead);
        mBoard[1].create(mBoardWidth, mBoardHeight, mColorDead);

        // for(unsigned i = 0; i < mBoardWidth; i++) {                                                             // Randomly place living cells based on noise
        //     for(unsigned j = 0; j < mBoardHeight; j++) {
        //         if(distribution(generator) == 0) {
        //             mBoard[mCurrentBoard].setPixel(i, j, mColorLiving);
        //         }
        //     }
        // }

        for(unsigned i = mBoardWidth / 2; i <  mBoardWidth / 2 + 5; i++) {
            for(unsigned j = mBoardHeight / 2; j <  mBoardHeight / 2 + 5; j++) {
                if(distribution(generator) == 0) {
                    mBoard[mCurrentBoard].setPixel(i, j, mColorLiving);
                }
            }
        }

        // unsigned j = mBoardHeight / 2;
        // std::string line;

        // while(getline(std::cin, line)) {
        //     for(unsigned i = mBoardWidth / 2; i <  mBoardWidth / 2 + line.length(); i++) {
        //         if(line[i - mBoardWidth / 2] == '1') {
        //             mBoard[mCurrentBoard].setPixel(i, j, mColorLiving);
        //         }
        //     }

        //     j++;
        // }
    }

    bool update(unsigned x, unsigned width, unsigned y, unsigned height) {                                      // Parallelizable step function, applies rules defined in getNext
        for(unsigned i = x; i < width; i++) {
            for(unsigned j = y; j < height; j++) {
                mBoard[!mCurrentBoard].setPixel(i, j, getNext(i, j));
            }
        }

        return true;
    }

    sf::Color getNext(int i, int j) {
        sf::Color current = mBoard[mCurrentBoard].getPixel(i, j);
        int count = 0;

        for(int x = -mSearchRadius; x <= mSearchRadius; x++) {
            for(int y = -mSearchRadius; y <= mSearchRadius; y++) {
                if(x == 0 && y == 0) continue;
                
                int x_new = i + x;
                if(x_new < 0) x_new += mBoardWidth;
                else if(x_new >= mBoardWidth) x_new -= mBoardWidth;
                
                int y_new = j + y;
                if(y_new < 0) y_new += mBoardHeight;
                else if(y_new >= mBoardHeight) y_new -= mBoardHeight;
                
                if(mBoard[mCurrentBoard].getPixel(x_new, y_new) == mColorLiving) count++;
            }
        }

        if(current == mColorLiving) {
            if(count >=2 && count <= 3)
                return mColorLiving;
            else
                return mColorDead;
        }

        if(current == mColorDead) {       
            if(count == 3)
                return mColorLiving;
            else
                return mColorDead;
        }

        return current;
    }

private:
    sf::RenderWindow        mWindow;
    unsigned                mWindowWidth = 2560;
    unsigned                mWindowHeight = 1440;
    std::string             mWindowTitle = "Nick's 2D Cellular Automata";

    sf::Image               mBoard[2];
    unsigned                mCurrentBoard = 0;
    unsigned                mInverseBoardScale = 1;
    unsigned                mBoardWidth;
    unsigned                mBoardHeight;
    const int               mSearchRadius = 1;
    sf::Texture             mBoardTexture;
    sf::Sprite              mBoardSprite;
    bool                    mOutputPNG;

    sf::Color               mColorLiving = sf::Color(0, 200, 0);
    sf::Color               mColorDead = sf::Color(50, 0, 0);
    int                     mInverseSpawnFrequency = 2;

    unsigned                mUpdateThreadCount = 64;
    std::vector<int>        mThreadHeights;
    bool                    mPause = false;

    int                     mDrawFPSLimit = 0;
    int                     mUpdateFPSLimit = 0;

    float                   mMaxFPS = 0.0f;
    int                     mFrames = 0;

    unsigned                mSeed;
};