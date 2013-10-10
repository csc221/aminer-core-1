#include "glog/logging.h"
#include "interface.pb.h"
#include "event.hpp"
#include "entity_searcher.hpp"
#include "search_service.hpp"
#include "aminer_data.hpp"
#include "indexed_graph_cache.hpp"

DEFINE_string(aminer, "aminer", "aminer data prefix");

using namespace std;
using namespace mashaler;
using namespace indexing;
using namespace sae::io;
using namespace zrpc;

struct AuthorService : public SearchServiceBase {
    AuthorService(IndexedGraph* ig) : SearchServiceBase(ig) {
    }

    // reimplemented datacenter services
    SERVICE(AuthorService_searchAuthors, EntitySearchRequest, EntitySearchResponse) {
        auto searcher = EntitySearcher(ig);
        auto results = searcher.search(WeightedType{{"Author", 1.0}}, WeightedType{{"Author", 1.0}, {"Publication", 1.0}}, request.query());
        fillSearchResponse(request, response, results);
        return true;
    }

    //TODO
    SERVICE(AuthorService_authorSearchSuggest, EntitySearchRequest, EntitySearchResponse) {
        auto searcher = EntitySearcher(ig);
        auto results = searcher.search(WeightedType{{"Author", 1.0}}, WeightedType{{"Author", 1.0}, {"Publication", 1.0}}, request.query());
        fillSearchResponse(request, response, results);
        return true;
    }

    SERVICE(AuthorService_getEntityById, EntitySearchRequest, EntitySearchResponse) {
        // auto aid = stoi(request.query());
        // response.set_entity_id(aid);

        // auto vit = ig->g->Vertices();
        // vit->MoveTo(aid);
        // fillEntity(response.add_entity(), vi.get());
        auto searcher = EntitySearcher(ig);
        auto results = searcher.get(WeightedType{{"Author", 1.0}}, WeightedType{{"Author", 1.0}, {"Publication", 1.0}}, request.query());
        fillSearchResponse(request, response, results);
        return true;
    }

    SERVICE(AuthorService_getEntityWithSameNameByName, EntitySearchRequest, EntitySearchResponse) {
        auto searcher = EntitySearcher(ig);
        auto results = searcher.search(WeightedType{{"Author", 1.0}}, WeightedType{{"Author", 1.0}, {"Publication", 1.0}}, request.query());
        fillSearchResponse(request, response, results);
        return true;
    }

    SERVICE(InfluenceSearchByAuthor, EntitySearchRequest, InfluenceSearchResponse) {
        auto aid = stoi(request.query());
        response.set_entity_id(aid);

        auto vit = ig->g->Vertices();
        vit->MoveTo(aid);
        for (auto eit = vit->OutEdges(); eit->Alive(); eit->Next()) {
            if (eit->TypeName() == "Influence") {
                auto ai = parse<AuthorInfluence>(eit->Data());
                Influence *inf = response.add_influence();
                inf->set_id(eit->TargetId());
                inf->set_topic(ai.topic);
                inf->set_score(ai.score);
            }
        }
        for (auto eit = vit->InEdges(); eit->Alive(); eit->Next()) {
            if (eit->TypeName() == "Influence") {
                auto ai = parse<AuthorInfluence>(eit->Data());
                Influence *inf = response.add_influenced_by();
                inf->set_id(eit->SourceId());
                inf->set_topic(ai.topic);
                inf->set_score(ai.score);
            }
        }
        return true;
    }
};

std::unique_ptr<AMinerData> IndexedGraphCache::ig;

#define ADD_METHOD(name) server->addMethod(#name, b(&AuthorService::name))

static void init(void *sender, void *args) {
    RpcServer *server = reinterpret_cast<RpcServer *>(args);
    LOG(INFO) << "loading aminer graph for author service.";
    IndexedGraphCache& gc = IndexedGraphCache::instance();
    auto *service = new AuthorService(gc.getGraph(FLAGS_aminer.c_str()));
    auto b = zrpc::make_binder(*service);
    ADD_METHOD(AuthorService_searchAuthors);
    ADD_METHOD(AuthorService_authorSearchSuggest);
    ADD_METHOD(AuthorService_getEntityById);
    ADD_METHOD(AuthorService_getEntityWithSameNameByName);
    ADD_METHOD(InfluenceSearchByAuthor);
    LOG(INFO) << "author service initialized.";
}

REGISTER_EVENT(init_authorservice, init);
        