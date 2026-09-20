# My First C++ Practice

## 자료구조, 알고리즘, 동시성, Boost.Asio Network Library 연습용

## C++14 version 실행 명령어
- g++ -std=c++14 -o executable source_file.cpp && ./executable && rm executable

## C++17 version 실행 명령어
- g++ -std=c++17 -o executable source_file.cpp && ./executable && rm executable

## C++20 version 실행 명령어
- g++ -std=c++20 -o executable source_file.cpp && ./executable && rm executable

## Boost 라이브러리를 써야 하는 경우
- CMakeLists.txt의 add_executable(...) 에 파일명(ex : 04-boost-asio/completion_token.cpp)을 집어 넣고 CLion에서 CMake 빌드 다시 실행 한 후, MyCppPractice configuration 선택해서 '>(run)' 버틑 눌러서 실행.
- CLion에서 CMakeLists.txt 를 인식하지 못할 경우엔 프로젝트 루트 디렉토리 내의 .idea 폴더 삭제후 다시 CLion 으로 오폰하면 Cmake Wizard 가 뜨면서 CMake project 로 프로젝트가 다시 빌드 됨.

## Windows 환경에서 실행하는 경우
- Visual Studio 2026 Community 버전을 설치한 후, Setting > Build,Execution,Deployment > ToolChains 에 Visual Studio 환경을 추가한다.
- Setting > Build,Eexcution,Deployment > CMake 에서 툴체인을 진전에 셋팅했던 Visual Studio로 설정한다. 이때, Generator를 반드시 default : detected Ninja 로 설정해줘야 한다. 그래야 MSVC 구현체를 정확하게 찾아낸다. 
