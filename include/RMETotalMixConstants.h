#ifndef RME_TOTALMIX_CONSTANTS_H
#define RME_TOTALMIX_CONSTANTS_H

#include <MidiMessage.h>

// static const constexpr MidiMessage::U7 NoteChannel;
// static const constexpr MidiMessage::U7 NoteBankLeft;
// static const constexpr MidiMessage::U7 NoteBankRight
// static const constexpr MidiMessage::U7 NoteChannelLeft;
// static const constexpr MidiMessage::U7 NoteChannelRight;
// static const constexpr MidiMessage::U7 NotePageUp;
// static const constexpr MidiMessage::U7 NotePageDown;
//
// static const constexpr MidiMessage::U7 AbsoluteHardwareSendLevelChannelOffset;
// static const constexpr MidiMessage::U7 AbsoluteSoftwareSendLevelChannelOffset;
// static const constexpr MidiMessage::U7 AbsoluteSubmixSendLevelChannelOffset;
// static const constexpr MidiMessage::U7 AbsoluteChannelControllerOffset;
//
// static const constexpr MidiMessage::U7 AbsoluteSubmixSelectChannelOffset;
// static const constexpr MidiMessage::U7 AbsoluteSubmitSelectControllerOffset;


    static const constexpr MidiMessage::U7 NoteChannel = 0;

    static const constexpr MidiMessage::U7 NoteBankLeft = 46;
    static const constexpr MidiMessage::U7 NoteBankRight = 47;
    static const constexpr MidiMessage::U7 NoteChannelLeft = 48;
    static const constexpr MidiMessage::U7 NoteChannelRight = 49;
    static const constexpr MidiMessage::U7 NotePageUp = 96;
    static const constexpr MidiMessage::U7 NotePageDown = 97;

    static const constexpr MidiMessage::U7 AbsoluteHardwareSendLevelChannelOffset = 1;
    static const constexpr MidiMessage::U7 AbsoluteSoftwareSendLevelChannelOffset = 5;
    static const constexpr MidiMessage::U7 AbsoluteSubmixSendLevelChannelOffset = 9;
    static const constexpr MidiMessage::U7 AbsoluteChannelControllerOffset = 102;

    static const constexpr MidiMessage::U7 AbsoluteSubmixSelectChannelOffset = 12;
    static const constexpr MidiMessage::U7 AbsoluteSubmitSelectControllerOffset = 102;

#endif /* RME_TOTALMIX_CONSTANTS_H */
