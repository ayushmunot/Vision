/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Vision.
 *
 * The Initial Developer of the Original Code is The Vision Team.
 * Portions created by The Vision Team are
 * Copyright (C) 1999, 2000, 2001 The Vision Team.  All Rights
 * Reserved.
 *
 * Contributor(s): Rene Gollent
 */

#include "PrefEvent.h"
#include "Vision.h"

#include <ScrollView.h>
#include <TextControl.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "PrefEvent"

static const char* EventControlLabels[] = {
	B_TRANSLATE("Join:"),	 B_TRANSLATE("Part:"),		 B_TRANSLATE("Nick:"),	B_TRANSLATE("Quit:"),
	B_TRANSLATE("Kick:"),	 B_TRANSLATE("Topic:"),	 B_TRANSLATE("Server notice:"), B_TRANSLATE("User notice:"),
	B_TRANSLATE("Notify on:"), B_TRANSLATE("Notify off:"), 0};

EventPrefsView::EventPrefsView(BRect frame)
	: BView(frame, "Event prefs", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS)
{
	AdoptSystemColors();
	BRect bounds(Bounds());
	bounds.left += 3;
	bounds.right -= B_V_SCROLL_BAR_WIDTH + 3;
	bounds.top += 3;
	bounds.bottom -= 5;
	int32 i(0);

	float label_width(0.0);

	for (i = 0; EventControlLabels[i]; ++i)
		if (StringWidth(EventControlLabels[i]) > label_width)
			label_width = StringWidth(EventControlLabels[i]);

	BView* bgView(new BView(bounds, "", B_FOLLOW_ALL_SIDES, B_WILL_DRAW));
	bgView->AdoptSystemColors();
	fEvents = new BTextControl* [MAX_EVENTS];

	for (i = 0; i < MAX_EVENTS; ++i) {
		fEvents[i] = new BTextControl(
			BRect(5, be_plain_font->Size() + ((1.5 * i) * 1.5 * be_plain_font->Size()),
				  5 + bounds.right - be_plain_font->StringWidth("gP"),
				  be_plain_font->Size() + (1.5 * (i + 1) * 1.5 * be_plain_font->Size())),
			"commands", EventControlLabels[i], vision_app->GetEvent(i).String(), NULL,
			B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);

		fEvents[i]->SetDivider(label_width + 5);

		BMessage* msg(new BMessage(M_EVENT_MODIFIED));

		msg->AddInt32("which", i);
		fEvents[i]->SetModificationMessage(msg);
		bgView->AddChild(fEvents[i]);
	}
	fScroller = new BScrollView("command fScroller", bgView, B_FOLLOW_ALL_SIDES,
		0, false, true, B_NO_BORDER);
	BScrollBar* bar(fScroller->ScrollBar(B_VERTICAL));

	fMaxheight = bgView->Bounds().Height();
	fProportionheight = fEvents[MAX_EVENTS - 1]->Frame().bottom + 10.0;
	bar->SetRange(0.0, (fProportionheight - fScroller->Bounds().Height()));
	bar->SetProportion(fScroller->Bounds().Height() / fProportionheight);

	AddChild(fScroller);
}

EventPrefsView::~EventPrefsView()
{
	delete[] fEvents;
}

void EventPrefsView::AttachedToWindow()
{
	BView::AttachedToWindow();
	for (int32 i = 0; i < MAX_EVENTS; i++) fEvents[i]->SetTarget(this);

	BScrollBar* bar(fScroller->ScrollBar(B_VERTICAL));
	if (bar) bar->SetSteps(3.0, 5.0);
}

void EventPrefsView::AllAttached()
{
	BView::AllAttached();
}

void EventPrefsView::FrameResized(float width, float height)
{
	BView::FrameResized(width, height);
	BScrollBar* bar(fScroller->ScrollBar(B_VERTICAL));
	if (!bar) return;
	float min, max, scrollheight(fScroller->Bounds().Height());

	bar->GetRange(&min, &max);
	if (scrollheight < fProportionheight) {
		if (max != fMaxheight) bar->SetRange(0.0, fProportionheight - scrollheight);
		bar->SetProportion(scrollheight / fProportionheight);
	} else {
		bar->SetProportion(1.0);
		bar->SetRange(0.0, 0.0);
	}
}

void EventPrefsView::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	case M_EVENT_MODIFIED: {
		int32 which;

		msg->FindInt32("which", &which);
		vision_app->SetEvent(which, fEvents[which]->TextView()->Text());
	} break;

	default:
		BView::MessageReceived(msg);
		break;
	}
}
