
/**
 * libemulation
 * Composite Monitor
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a composite monitor.
 */

#include "OEComponent.h"

class CompositeMonitor : public OEComponent
{
public:
	CompositeMonitor();
	
	bool setValue(string name, string value);
	bool getValue(string name, string &value);
	bool setRef(string name, OEComponent *ref);
	bool init();
	
private:
	OEComponent *emulation;
	OEComponent *canvas;
	
	float compositeLumaCutoff;
	float compositeChromaCutoff;
	float compositeHue;
	float compositeSaturation;
	bool compositeColorize;
	int compositeDecoderMatrix;
	float screenBrightness;
	float screenContrast;
	float screenRedGain;
	float screenGreenGain;
	float screenBlueGain;
	float screenBarrel;
	float screenPersistance;
	float screenHorizontalCenter;
	float screenHorizontalSize;
	float screenVerticalCenter;
	float screenVerticalSize;
};
