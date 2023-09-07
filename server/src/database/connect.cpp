#include <iostream>
#include <pqxx/pqxx>

const char* DB_NAME = std::getenv("POSTGRES_DB");
const char* DB_USER = std::getenv("POSTGRES_USER");
const char* DB_PASSWORD = std::getenv("POSTGRES_PASSWORD");
const char* DB_HOST = std::getenv("DB_HOST");

int createConnection()
{
    try
    {
        // Connect to the db
        std::stringstream connectionParams;
        connectionParams << "dbname=" << DB_NAME << " user=" << DB_USER << " password=" << DB_PASSWORD << " host=" << DB_HOST << " port=5432";
        pqxx::connection c(connectionParams.str());
        std::cout << "Connected to " << c.dbname() << '\n';
    }
    catch (std::exception const &e)
    {
        std::cerr << "ERROR: " << e.what() << '\n';
        return 1;
    }
    return 0;
}