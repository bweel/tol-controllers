#ifndef ILINKABLE_H
#define	ILINKABLE_H

template <class T>
class ILinkable
{
	public:
		virtual void link_to(T *) = 0;
		virtual void sync_to() = 0;
		virtual void sync_from() = 0;
	protected:
		virtual ~ILinkable() {};
};

#endif	/* ILINKABLE_H */