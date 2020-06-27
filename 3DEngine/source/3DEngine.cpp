//*****************************************************//
// Coleman Jonas
// 3DEngine
//*****************************************************//

#include <pch.h>
#include "engine.h"


int main(int argc, char* argv[])
{
  //std::cout << "test" << std::endl;
  try
  {
    return engineRun(); // (argc, argv);
  }
  catch (const std::exception& e)
  {
    MessageBox(GetActiveWindow(), e.what(), "Error", MB_OK | MB_ICONERROR);
    return EXIT_FAILURE;
  }
  catch (const std::string& e)
  {
    MessageBox(GetActiveWindow(), e.c_str(), "Error", MB_OK | MB_ICONERROR);
    return EXIT_FAILURE;
  }
  catch (const char* e)
  {
    MessageBox(GetActiveWindow(), e, "Error", MB_OK | MB_ICONERROR);
    return EXIT_FAILURE;
  }
  catch (...)
  {
    MessageBox(GetActiveWindow(), "An error has occurred.", "Error", MB_OK | MB_ICONERROR);
    return EXIT_FAILURE;
  }
}

