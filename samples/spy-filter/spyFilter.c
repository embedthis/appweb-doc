/*
    spyFilter.c - Eavesdrop on input data

    This sample filter peeks at input data and sets a response header if the word "hello" is found.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "http.h"

/*********************************** Code *************************************/

static int matchSpy(HttpStream *stream, HttpRoute *route, int dir)
{
    if (stream->rx->form && smatch(stream->rx->pathInfo, "/index.html")) {
        return HTTP_ROUTE_OK;
    }
    return HTTP_ROUTE_REJECT;
}


static void incomingSpy(HttpQueue *q, HttpPacket *packet)
{
    if (packet->content == 0) {
        /*
            End of input. Set a greeting response header if the input says hello.
         */
        if (q->first && q->first->content && scontains(q->first->content->start, "hello")) {
            httpSetHeader(q->stream, "X-Greeting", "found");
        } else {
            httpSetHeader(q->stream, "X-Greeting", "missing");
        }
        httpPutPacketToNext(q, packet);

    } else {
        /* Join all input together to make scanning easier */
        httpJoinPacketForService(q, packet, 0);
    }
}


MprModule *httpSpyFilterInit(Http *http, MprModule *module)
{
    HttpStage     *filter;

    if ((filter = httpCreateFilter("spyFilter", module)) == 0) {
        return 0;
    }
    filter->match = matchSpy;
    filter->incoming = incomingSpy;
    return module;
}

/*
    Copyright (c) Embedthis Software. All Rights Reserved.
    This software is distributed under commercial and open source licenses.
    You may use the Embedthis Open Source license or you may acquire a
    commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.md distributed with
    this software for full details and other copyrights.
 */
