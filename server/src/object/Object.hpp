#include <string>

class Object
{
public:
  Object(int id, std::string type, std::string name);

  const int getId();
  const int getInstanceId();
  const std::string getType();
  const std::string getName();

private:
  int idM;         // Unique id for object type (For example, tree, rock, etc)
  int instanceIdM; // Unique id for object instance (For example, tree 1, tree at 2 etc)
  std::string typeM;
  std::string nameM;

  static int nextInstanceId;
};