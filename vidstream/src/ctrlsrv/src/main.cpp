#include <ctrlsrv.hpp>

#include <boost/thread.hpp>
#include <boost/foreach.hpp>

using namespace boost::property_tree;
using namespace std;

void display(const int depth, const ptree& tree) {
    BOOST_FOREACH( ptree::value_type const&v, tree.get_child("") ) {
        ptree subtree = v.second;
        string nodestr = tree.get<string>(v.first);

        // print current node
        cout << string("").assign(depth*2,' ') << "* ";
        cout << v.first;
        if ( nodestr.length() > 0 )
            cout << "=\"" << tree.get<string>(v.first) << "\"";
        cout << endl;

        // recursive go down the hierarchy
        display(depth+1,subtree);
    }
}

int main(int argc, char *argv[])
{
    cfg_ptr_t pt(new boost::property_tree::ptree());
    pt->put("cfg.dataport", 9955);
    pt->put("cfg.bch.m", 5);
    pt->put("cfg.bch.t", 103);
    pt->put("cfg.img.width", 640);
    pt->put("cfg.img.height", 480);
    pt->put("cfg.img.rst", 1);
    pt->put("cfg.img.lum", 20);
    pt->put("cfg.img.chrom", 20);
    pt->put("cfg.img.bw", false);

    bool stop_flag = false;
    std::string url("tcp://127.0.0.1:9901");
    ctrlsrv cfgsrv(pt, url, stop_flag);
    boost::thread t1(cfgsrv);

    // now test it
    int socket_ = nn_socket (AF_SP, NN_REQ);
    int session_ = nn_connect (socket_, url.c_str());

    char cmd[] = "config";
    int bytes = nn_send(socket_, cmd, 6, 0);

    char * buf = NULL;
    bytes = nn_recv (socket_, &buf, NN_MSG, 0);
    if (bytes >= 0)
    {
        std::string data(buf, buf+bytes);
        std::stringstream ss(data);
        boost::property_tree::ptree rcvd;
        boost::property_tree::read_json(ss, rcvd);

        display(3, rcvd);
    }
    else
    {
        std::cerr << "error receiving config" << std::endl;
    }

    cfgsrv.stop();
    // to unblock receive
    nn_send(socket_, cmd, 6, 0);
    nn_shutdown(socket_, session_);
    nn_close(socket_);
    nn_term();
    t1.join();


    return 0;
}
