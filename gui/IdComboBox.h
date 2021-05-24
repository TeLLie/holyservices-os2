#ifndef IDCOMBOBOX_H_INCLUDED_19948FE5_1C61_47D2_9B71_D416F87EFF44
#define IDCOMBOBOX_H_INCLUDED_19948FE5_1C61_47D2_9B71_D416F87EFF44

///\class IdComboBox
/// Class is ancestor of combo boxes holding list of 
/// items given by a table model holding string about CIdTag
/// in Qt::UserRole of a row
///\date 12-2009
///\author Jan 'Kovis' Struhar
#include <QComboBox>

class IdTag;

class IdComboBox : public QComboBox
{
	Q_OBJECT
	typedef QComboBox super;
public:
    virtual ~IdComboBox();

	/// usual table view for combo popups
	static QAbstractItemView* createViewForComboPopup(const QSize& minSize);

	/// make the item with given id current one
	void setCurrentId(const IdTag&);

	/// usually textual representation of CIdTag of current item, but sometimes
	/// also something else
	virtual QString currentIdString() const;
	IdTag currentId() const;

	/// implement combo items filling 
	virtual void fillCombo() = 0;

protected:
    IdComboBox(QWidget* p);
};

#endif

