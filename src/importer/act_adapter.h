#pragma once
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "JConfMap.h"
#include "analyzer/ArnetAnalyzer.h"
#include "Logger.h"

#define ACT_CACHE_UNINIT -2

using std::string;
using std::vector;
using std::pair;

struct ACTParam
{
	double alpha;
	double beta;
	int ntopics;
	int nauthors;
	int nconfs;
	int ndocs;
	int nwords;
	int nwordtoken;
	int liter;
};


class ACT
{
public:

	const double PRECISION;
	const int FILEBUFFERSIZE;
	const int STRINGBUFFERSIZE;

	ACT();
	~ACT();

	ACTParam parameter;

	void load();

	void addACTcount(int tid, int actxid, vector<unordered_map<int, int>>* actMap, int* sumMap);
	int getACTcount(int tid, int actxid, char xtype);

	int getAuthorNaIdByACTaid(int actaid);
	int getConfIdByACTcid(int actcid);
	int getPubIdByACTpid(int actpid);
	string getWordByACTwid(int actwid);

	int getACTaidByAuthorNaId(int naid);
	int getACTcidByConfid(int confid);
	int getACTpidByPubId(int pubid);
	int getACTwidByWord(string word);

	unordered_map<int, int>& getAmap(int tid);
	unordered_map<int, int>& getCmap(int tid);
	unordered_map<int, int>& getPmap(int tid);
	unordered_map<int, int>& getWmap(int tid);

	JConfMap* jConfMapInstance;

private:
	int nTopic;
	int nWords;
	int nPublications;
	int nAuthors;
	int nConfs;

	vector<unordered_map<int, int>> _ACTa2countMap;
	vector<unordered_map<int, int>> _ACTc2countMap;
	vector<unordered_map<int, int>> _ACTp2countMap;
	vector<unordered_map<int, int>> _ACTw2countMap;

	int* _sumA2Count;
	int* _sumC2Count;
	int* _sumP2Count;
	int* _sumW2Count;

	unordered_map<int, string> _authormap;
	unordered_map<int, string> _confmap;
	unordered_map<int, int> _docmap;
	unordered_map<int, string> _wordmap;

	unordered_map<string, int> _reauthormap;
	unordered_map<string, int> _reconfmap;
	unordered_map<int, int> _redocmap;
	unordered_map<string, int> _rewordmap;

	void _loadTAssign(string path);
	void _loadOthers(string path);
	void _loadAuthorMap(string path);
	void _loadConfMap(string path);
	void _loadDocMap(string path);
	void _loadWordMap(string path);

	void _init();
private:

	//naid to aid in act model
	vector<int> act_aid_map;
	std::mutex act_aid_map_mutex;
	//conf_id to cid of act model
	vector<int> act_conf_map;
	std::mutex act_conf_map_mutex;
};

class ACTadapter
{
public:
	static ACTadapter* getInstance();

	ACTadapter();
	~ACTadapter();

	ACTParam& getParameter(); //���ص�ǰmodel���õĲ���������topic�����ʱ�size������������������������������������������alpha��beta��lowerbound
	vector<double> getTopicDistributionGivenAuthor(int naid); //����P(topic_i|author)���������ж�ĳ���������ĸ�topic�������߶�Ӧ��topic
	vector<double> getTopicDistributionGivenAuthor(string name); //ͬ�ϣ������Ϊ��������
	vector<double> getTopicDistributionGivenConf(int confid); //����P(topic_i|conf)���������ж�ĳ���������ĸ�topic���һ����Ӧ��topic
	vector<double> getTopicDistributionGivenConf(string confname); //ͬ�ϣ������Ϊ��������
	vector<double> getTopicDistributionGivenPub(int pubid, Publication const * pub = nullptr); //����P(topic_i|pub)���������ж�ĳ���������ĸ�topic�������Ķ�Ӧ��topic
	vector<double> getTopicDistributionGivenQuery(string query); //����P(topic_i|query) = sigma_n(P(topic_i|word_j))/n���������ж�ĳquery�����ĸ�topic����query��Ӧ��topic��query�Է�Ӣ����ĸ�ָ�����ʱ������Ҫ�ȹ��˷Ƿ��ַ���stem

	//�⼸����������������ܴ󣬼�������lowerbound��
	unordered_map<int, double> getAuthorDistributionGivenTopic(int tid); //���ص�Key��naid��Value��P(author_i|topic)��ϡ���ʾ����������ĳ��topic����ص�����
	unordered_map<int, double> getConfDistributionGivenTopic(int tid); //���ص�Key��confid��Value��P(conf_i|topic)��ϡ���ʾ����������ĳ��topic����صĻ���
	unordered_map<int, double> getPubDistributionGivenTopic(int tid); //���ص�Key��pid��Value��P(pub_i|topic)��ϡ���ʾ����������ĳ��topic����ص����ģ�Ҳ��ͨ��Map��size��֪topic�ķ�Χ��С
	unordered_map<string, double> getWordDistributionGivenTopic(int tid); //���ص�Key��word��Value��P(word_i|topic)��ϡ���ʾ����������ĳ��topic����صĵ���

	//�����¼ӵķ��� by Hang Su<su_hang@live.com> [July 23, 2011]
public:
	bool getTopTopicGivenAuthor(vector<pair<int, double>>& result, int naid);
	bool getTopTopicGivenPub(vector<pair<int, double>>& result, int pub_id);
	bool getTopTopicGivenConf(vector<pair<int, double>>& result, int conf_id);

	double getQueryProbabilityGivenAuthor(vector<string>& words, int naid);
	double getWordProbabilityGivenAuthor(string& word, int naid);
private:
	bool getTopTopic(vector<pair<int, double>>& result, vector<pair<int, double>>& distribution);
public:
	const int MAX_TOP_TOPIC_COUNT;
	const double TOPIC_THRESHOLD;
private:
	ACT _act;
};
