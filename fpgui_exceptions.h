#ifndef FPGUI_EXCEPTIONS_H
#define FPGUI_EXCEPTIONS_H

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <QString>

#ifndef __SHORT_FORM_OF_FILE__

#ifdef WIN32
#define __SHORT_FORM_OF_FILE_WIN__ \
    (strrchr(__FILE__,'\\') \
    ? strrchr(__FILE__,'\\')+1 \
    : __FILE__ \
    )
#define __SHORT_FORM_OF_FILE__ __SHORT_FORM_OF_FILE_WIN__
#else
#define __SHORT_FORM_OF_FILE_NIX__ \
    (strrchr(__FILE__,'/') \
    ? strrchr(__FILE__,'/')+1 \
    : __FILE__ \
    )
#define __SHORT_FORM_OF_FILE__ __SHORT_FORM_OF_FILE_NIX__
#endif

#endif

#define THROW(exception_type) { throw exception_type(__FUNCTION__, __SHORT_FORM_OF_FILE__, __LINE__); }
#define THROWM(exception_type, message) { throw exception_type(__FUNCTION__, __SHORT_FORM_OF_FILE__, __LINE__, message); }

#ifdef _LINUX
#define NO_ITOA
#endif

#ifdef __APPLE__
#define NO_ITOA
#endif

#ifdef NO_ITOA
inline char *itoa(long i, char* s, int /*dummy_radix*/) {
    sprintf(s, "%ld", i);
    return s;
}
#endif

namespace fpgui { namespace exceptions {

class Generic_Exception
{
    Q_DECLARE_TR_FUNCTIONS(Generic_Exception)

    public:

        Generic_Exception(const QString& facility, const QString& file = "", int line = 0, const QString& message = ""):
        facility_(facility),
        message_(message),
        file_(file),
        line_(line)
        {
        }

        QString what()
        {
            char buf[256];

            #ifdef _WIN32
                _itoa_s(line_, buf, sizeof(buf) / sizeof(char), 10);
            #else
                itoa(line_, buf, 10);
            #endif

            return "[" + facility_ + ", f:" + file_ + ", l:" + buf + "] " + message_;
        }

        QString facility_, message_, file_;
        int line_;


    private:

        Generic_Exception();
};

class Incorrect_Parameter: public Generic_Exception
{
    public:

        Incorrect_Parameter(const QString& facility, const QString& file = "", int line = 0, const QString& message = tr("Parameter supplied to function has incorrect value.")):
        Generic_Exception(facility, file, line, message)
        {
        }
};

}}

#endif // FPGUI_EXCEPTIONS_H
