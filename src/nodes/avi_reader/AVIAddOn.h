//
//
// ABH missing from original source
//
//

#if !defined __AVIADDON_H__
#define __AVIADDON_H__

#include <MediaNode.h>

class AVIAddOn : public BMediaAddOn {

public:
	AVIAddOn(image_id);
	~AVIAddOn();
// ABH	virtual BMediaAddOn *make_media_addon(image_id);

private:
	status_t InitCheck(const char **);
	int32 CountFlavors();
	status_t GetFlavorAt(int32, flavor_info**);
	BMediaNode *InstantiateNodeFor(const flavor_info *, BMessage *, status_t *);
	status_t GetConfigurationFor(BMediaNode *, BMessage *);
	bool WantsAutoStart();
	status_t SniffRef(const entry_ref &, BMimeType *, float *, int32*);
	status_t SniffType(const BMimeType &, float *, int32 *);
	status_t AutoStart(int, BMediaNode**, int32*, bool*);

	 int32 kNumFlavors;


};


#endif

