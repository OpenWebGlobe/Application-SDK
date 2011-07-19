
#include "RenderFactory.h"
#include "win32/RenderWindowGL.h"

namespace OpenGlobe
{

   shared_ptr<IRenderWindow> RenderFactory::RenderWindow(const OptionVector& options)
   {
       shared_ptr<IRenderWindow> qWindow;

       // parse options to decide which window is created.
       // right now there is only "RenderWindowGL" available, so no decision is required

       qWindow = shared_ptr<IRenderWindow>(new RenderWindowGL());
       qWindow->SetOptions(options);
       return qWindow;
   }
   

   


}