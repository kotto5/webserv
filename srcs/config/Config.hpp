#ifndef CONFIG_HPP
#define CONFIG_HPP


class Config
{
	public:
		Config();
		~Config();
		Config* get_instance();

	private:
		static Config* _instance;
};

#endif