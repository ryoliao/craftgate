
#include "CGGraph_PCH.h"

namespace cg
{

CGTLS static IEventHandler* t_Handler = 0;
CGTLS static c8 t_emBuffer[4096] = "";

void CGError::setHandler( IEventHandler* handler )
{
    if (handler)
        handler->grab();
    if (t_Handler)
        t_Handler->drop();

    t_Handler = handler;
}

void CGError::error(c8 const * format, ...)
{
    SEvent e;
    e.EventType = ECET_ERROR;

    va_list ap;
    va_start(ap, format);
    vsprintf_s(t_emBuffer, format, ap);
    va_end(ap);

    e.Message = t_emBuffer;

    if (t_Handler)
        t_Handler->OnEvent(e);
    else
        ::OutputDebugStringA(t_emBuffer);
}

void CGError::warning(c8 const * format, ...)
{
    SEvent e;
    e.EventType = ECET_WARNING;

    va_list ap;
    va_start(ap, format);
    vsprintf_s(t_emBuffer, format, ap);
    va_end(ap);

    e.Message = t_emBuffer;

    if (t_Handler)
        t_Handler->OnEvent(e);
    else
        ::OutputDebugStringA(t_emBuffer);
}

void CGError::message(c8 const * format, ...)
{
    SEvent e;
    e.EventType = ECET_MESSAGE;

    va_list ap;
    va_start(ap, format);
    vsprintf(t_emBuffer, format, ap);
    va_end(ap);

    e.Message = t_emBuffer;

    if (t_Handler)
        t_Handler->OnEvent(e);
    else
        ::OutputDebugStringA(t_emBuffer);
}

} // namespace cg