
/**
 * libappleiigo
 * Event Controller
 * (C) 2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include "AppleIIEventController.h"

void EventController::sendMessage(void * messageData)
{
	struct EventMessage &message = *((struct EventMessage *) messageData);
	switch (message.command)
	{
		case MSG_RENDERFRAME:
			setEvent(EVENT_FRAMESTARTED);
			
/*			for (verticalCountIndex = verticalCountStart;
				 verticalCountIndex < VERT_VBL;
				 verticalCountIndex++)
			{
				for (horizontalIndex = 0;
					 horizontalIndex < cpuClockPerScanline;
					 horizontalIndex++)
					setEvent(EVENT_CPUCLOCKASSERTED);
				frameClockIndex += CLOCK14M_SCANLINE;
				q3ClockIndex += HORIZ_NUM * 2;
			}
			
			setEvent(EVENT_FRAMEIRQASSERTED);
			
			for (; verticalCountIndex < VERT_END; verticalCountIndex++)
			{
				cpu->executeInstructions();
				frameClockIndex += CLOCK14M_SCANLINE;
				q3ClockIndex += HORIZ_NUM * 2;
				videoRenderer->onScanline(getScanlineIndex());
			}
			
			verticalCountIndex = VERT_END - 1;
			clockIndex += frameClockIndex;
			frameClockIndex = 0;*/
			setEvent(EVENT_FRAMEENDED);
			break;
		case MSG_RESTART:
			setEvent(EVENT_COMPUTERRESTARTED);
			break;
		case MSG_ASSERTRESET:
			setEvent(EVENT_RESETASSERTED);
			break;
		case MSG_CLEARRESET:
			setEvent(EVENT_RESETCLEARED);
			break;
		case MSG_ASSERTIRQ:
			setEvent(EVENT_IRQASSERTED);
			break;
		case MSG_CLEARIRQ:
			setEvent(EVENT_IRQCLEARED);
			break;
		case MSG_ASSERTNMI:
			setEvent(EVENT_NMIASSERTED);
			break;
		case MSG_SETPALTIMING:
			break;
		case MSG_ISPALTIMING:
			break;
		case MSG_SETCPUCLOCKFACTOR:
			break;
		case MSG_GETCPUCLOCKFACTOR:
			break;
		case MSG_GETTIMER:
			break;
		case MSG_SETTIMER:
			break;
	}
}
