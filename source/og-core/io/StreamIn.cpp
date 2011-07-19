
#include "StreamIn.h"
#include "StreamInFile.h"
#include "StreamInHttp.h"
#include "string/FilenameUtils.h"
#include "ogException.h"

/*boost::shared_ptr<IStreamIn> CreateStreamInFromURL(const std::string& sURL)
{
   boost::shared_ptr<IStreamIn> nullStream;
   boost::shared_ptr<IStreamIn> qStreamIn;

   if (FilenameUtils::IsFileOnWeb(sURL))
   {
      qStreamIn = boost::shared_ptr<IStreamIn>(new StreamInHttp());
      if (qStreamIn)
      {
         try
         {
            ((StreamInHttp*)qStreamIn.get())->Open(sURL);
         }
         catch (ogException e)
         {
            return nullStream;
         }
      }
   }
   else
   {
      qStreamIn = boost::shared_ptr<IStreamIn>(new StreamInFile());
      if (qStreamIn)
      {
         try
         {
            ((StreamInFile*)qStreamIn.get())->Open(sURL);
         }
         catch (ogException e)
         {
            return nullStream;
         }
      }
   }

   return qStreamIn;
}
*/
