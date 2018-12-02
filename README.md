# Atmel-XMEGA-drivers
Source files for Atmel XMEGA controllers
## Recomended folder structure in Atmel Studio 7
- **app/**
  - tasks/
    - *task_startup_app.cpp*
  - app.h
- **conf/**
  - *conf_board.h*
  - *conf_multitask.h*
- **core/**
  - drivers/
    - *clock.h*
  - extensions/
  - modules/
  - multitask/
    - *multitask.cpp*
    - *multitask.h*
  - tasks/
    - *task_startup_core.cpp*
  - core.h
- *project_name.atsln*
- *project_name.cppproj*
- *project_name.componentinfo.xml*
