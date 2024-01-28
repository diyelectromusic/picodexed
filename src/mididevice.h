#ifndef _mididevice_h
#define _mididevice_h

#include <cstdint>
#include <cstddef>

class CPicoDexed;

class CMIDIDevice
{
public:
	const uint8_t MIDIDisabled=0;
	const uint8_t MIDIOmni=255;

public:
	CMIDIDevice (CPicoDexed *pSynth);
	~CMIDIDevice (void);

	void SetChannel (uint8_t ucChannel);
	uint8_t GetChannel (void);

	virtual void Send (const uint8_t *pMessage, size_t nLength) {}

protected:
	void MIDIMessageHandler (const uint8_t *pMessage, size_t nLength);
private:
	CPicoDexed *m_pSynth;
	uint8_t m_ucChannel;
};

#endif
