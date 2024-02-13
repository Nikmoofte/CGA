#include "ObjParser.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <iterator>
#include <thread>
#include <array>

#include "config.hpp"

#define NOTIMER

#ifdef TIMER
#include <chrono>
#include <Windows.h>
#endif


Object ObjParser::operator()(const std::string &pathToFile)
{
    OUTPUT_IF_DEBUG_("ObjParser start parsing file: " + pathToFile)
    using namespace std;
#ifdef TIMER
    auto beg = chrono::high_resolution_clock::now();
#endif

    array<thread, 4> threads;

    ifstream file(pathToFile);
    file.seekg(0, ios::end);
    size_t size = file.tellg();
    string fileString(size, ' ');
    file.seekg(0);
    file.read(&fileString.front(), size);

    Object obj;

    threads[0] = thread([&obj, &fileString](){
        size_t pos = 0;
        while((pos = fileString.find("v ", pos)) != string::npos)
        {
            size_t end = fileString.find('\n', pos);
            string line = fileString.substr(pos, end - pos);
            istringstream ss(line);
            ss >> line;
            float x, y, z, w = 1.0f;
            ss >> x >> y >> z;
            obj.addVertex({x, y, z, w});
            pos = end;
        }
    });
    threads[1] = thread([&obj, &fileString](){
        size_t pos = 0;
        while((pos = fileString.find("vt ", pos)) != string::npos)
        {
            size_t end = fileString.find('\n', pos);
            string line = fileString.substr(pos, end - pos);
            istringstream ss(line);
            ss >> line;
            float u, v, w;
            ss >> u >> v >> w;
            obj.addTexture({u, v, w});
            pos = end;
        }
    });
    threads[2] = thread([&obj, &fileString](){
        size_t pos = 0;
        while((pos = fileString.find("vn ", pos)) != string::npos)
        {
            size_t end = fileString.find('\n', pos);
            string line = fileString.substr(pos, end - pos);
            istringstream ss(line);
            ss >> line;
            float x, y, z;
            ss >> x >> y >> z;
            obj.addNormal({x, y, z});
            pos = end;
        }
    });
    threads[3] = thread([&obj, &fileString](){
        size_t pos = 0;
        while((pos = fileString.find("f ", pos)) != string::npos)
        {
            size_t end = fileString.find('\n', pos);
            string line = fileString.substr(pos, end - pos);
            istringstream ss(line);
            ss >> line;
            vector<string> tokens(
                std::istream_iterator<string>{ss},
                std::istream_iterator<string>{}
            );

            array<tuple<size_t, size_t, size_t>, 3> face;
            for(int i = 0; i < 3; ++i)
            {
                string& token = tokens[i];
                size_t vertexInd = Object::NONE;
                size_t textureInd = Object::NONE;
                size_t normalInd = Object::NONE;

                replace(token.begin(), token.end(), '/', ' ');
                istringstream ss(token);
                ss >> vertexInd >> textureInd >> normalInd;
                face.at(i) = make_tuple(vertexInd -1, textureInd - 1, normalInd - 1);
            }
            obj.addFace(face);
            pos = end;
        }
    });

    for(auto& thread : threads)
        thread.join();
#ifdef TIMER
    chrono::duration<double> time = chrono::high_resolution_clock::now() - beg;
    MessageBox(NULL, ("Time: " + to_string(time.count())).c_str(), "Time", MB_OK);
#endif
    return obj;
}

Object ObjParser::operator()(const std::string &pathToFile, BS::thread_pool &threads)
{
    OUTPUT_IF_DEBUG_("ObjParser start parsing file: " + pathToFile)
    using namespace std;
#ifdef TIMER
    auto beg = chrono::high_resolution_clock::now();
#endif

    array<std::future<void>, 4> futures;

    ifstream file(pathToFile);
    file.seekg(0, ios::end);
    size_t size = file.tellg();
    string fileString(size, ' ');
    file.seekg(0);
    file.read(&fileString.front(), size);

    Object obj;

    futures[0] = threads.submit_task([&obj, &fileString](){
        size_t pos = 0;
        while((pos = fileString.find("v ", pos)) != string::npos)
        {
            size_t end = fileString.find('\n', pos);
            string line = fileString.substr(pos, end - pos);
            istringstream ss(line);
            ss >> line;
            float x, y, z, w = 1.0f;
            ss >> x >> y >> z;
            obj.addVertex({x, y, z, w});
            pos = end;
        }
    });
    futures[1] = threads.submit_task([&obj, &fileString](){
        size_t pos = 0;
        while((pos = fileString.find("vt ", pos)) != string::npos)
        {
            size_t end = fileString.find('\n', pos);
            string line = fileString.substr(pos, end - pos);
            istringstream ss(line);
            ss >> line;
            float u, v, w;
            ss >> u >> v >> w;
            obj.addTexture({u, v, w});
            pos = end;
        }
    });
    futures[2] = threads.submit_task([&obj, &fileString](){
        size_t pos = 0;
        while((pos = fileString.find("vn ", pos)) != string::npos)
        {
            size_t end = fileString.find('\n', pos);
            string line = fileString.substr(pos, end - pos);
            istringstream ss(line);
            ss >> line;
            float x, y, z;
            ss >> x >> y >> z;
            obj.addNormal({x, y, z});
            pos = end;
        }
    });
    futures[3] = threads.submit_task([&obj, &fileString](){
        size_t pos = 0;
        while((pos = fileString.find("f ", pos)) != string::npos)
        {
            size_t end = fileString.find('\n', pos);
            string line = fileString.substr(pos, end - pos);
            istringstream ss(line);
            ss >> line;
            vector<string> tokens(
                std::istream_iterator<string>{ss},
                std::istream_iterator<string>{}
            );

            array<tuple<size_t, size_t, size_t>, 3> face;
            for(int i = 0; i < 3; ++i)
            {
                string& token = tokens[i];
                size_t vertexInd = Object::NONE;
                size_t textureInd = Object::NONE;
                size_t normalInd = Object::NONE;

                replace(token.begin(), token.end(), '/', ' ');
                istringstream ss(token);
                ss >> vertexInd >> textureInd >> normalInd;
                face.at(i) = make_tuple(vertexInd -1, textureInd - 1, normalInd - 1);
            }
            obj.addFace(face);
            pos = end;
        }
    });

    for(auto& f : futures)
        f.wait();


#ifdef TIMER
    chrono::duration<double> time = chrono::high_resolution_clock::now() - beg;
    MessageBox(NULL, ("Time: " + to_string(time.count())).c_str(), "Time", MB_OK);
#endif
    return obj;
}

/*Object ObjParser::operator()(const std::string &pathToFile)
{
    using namespace std;
    auto beg = chrono::high_resolution_clock::now();
    ifstream in(pathToFile);

    string line;
    Object obj;
    while(getline(in, line))
    {
        if(line.find_first_of("vtnf") != string::npos)
        {
            istringstream ss(line);
            ss >> line;
            int itype = type[0];
            itype += type[1];
            switch (itype)
            {
            case 118:
                {
                    float x, y, z, w;
                    ss >> x >> y >> z;
                    obj.addVertex({x, y, z, w});
                }
                break;   

            case 234:
                {
                    float u, v, w;
                    ss >> u >> v >> w;
                    obj.addTexture({u, v, w});
                }
            break;

            case 228:
            {
                float x, y, z;
                ss >> x >> y >> z;
                obj.addNormal({x, y, z});
            }
            break;           
            case 102:
                {
                    vector<string> tokens(
                        std::istream_iterator<string>{ss},
                        std::istream_iterator<string>{}
                    );

                    if(tokens.size() == 3)
                    {
                        array<tuple<size_t, size_t, size_t>, 3> face;
                        for(int i = 0; i < 3; ++i)
                        {
                            string& token = tokens[i];
                            size_t vertexInd = Object::NONE;
                            size_t textureInd = Object::NONE;
                            size_t normalInd = Object::NONE;

                            replace(token.begin(), token.end(), '/', ' ');
                            istringstream ss(token);
                            ss >> vertexInd >> textureInd >> normalInd;
                            face.at(i) = make_tuple(vertexInd -1, textureInd - 1, normalInd - 1);
                        }
                        obj.addFace(face);
                    }
                    else
                    {
                        cerr << "Error: unsupported face format" << endl;
                    }
                }
                break;
            }
        }
    }

    chrono::duration<double> time = chrono::high_resolution_clock::now() - beg;
    MessageBox(NULL, ("Time: " + to_string(time.count())).c_str(), "Time", MB_OK);
    return obj;
}*/
