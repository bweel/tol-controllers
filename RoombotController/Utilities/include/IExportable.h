#ifndef IEXPORTABLE_H
#define	IEXPORTABLE_H

#include <boost/property_tree/ptree.hpp>

class IExportable
{
	public:
		virtual boost::property_tree::ptree export_to_ptree() const = 0;
		virtual void import_from_ptree(const boost::property_tree::ptree &) = 0;
	protected:
		virtual ~IExportable() {};
};

#endif	/* IEXPORTABLE_H */