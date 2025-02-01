//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TEST_UNITTESTENVIROMENT_H
#define __OMEGA_TEST_UNITTESTENVIROMENT_H
//-------------------------------------------------------------------------------------------

#include <QString>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace test
{
//-------------------------------------------------------------------------------------------

class UnitTestEnviroment
{
	public:
		~UnitTestEnviroment();
		
		static UnitTestEnviroment *instance(const QString& unitExePath);
		static UnitTestEnviroment *instance();
		
		QString testFileName(const QString& name);
		
	private:
		static UnitTestEnviroment *m_instance;
		QString m_sourcePath;
		
		UnitTestEnviroment();
		void printError(const char *strR, const QString& err) const;
		bool init(const QString& unitExePath);
};

//-------------------------------------------------------------------------------------------
} // namespace test
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
