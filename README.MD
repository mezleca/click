elterclick

# TODO:
- [x] linux build
- [x] windows build
    - [x] replace sdl with glfw (sdl is too difficult for me on windows)
    - [x] mouse shit
        - [x] mouse hook
        - [x] mouse click simulation
    - [x] make sure linux build still works
- [ ] keyboard trigger/target
    - [x] windows kb hook
    - [x] linux kb event (prob just need to update mask)
    - [x] stop casting vKey to keylist (makes no sense)
    - [x] add kb max to keylist
    - [x] add kb_keypress method to linux
    - [ ] add kb_keypress method to windows
- [ ] extra
    - [x] improve Input::toString function
    - [x] use mingw64 instead of 32 on windows
    - [ ] test elterclick on windows/linux to see if it builds and works as expected
   
