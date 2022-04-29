#include "mdnssub.h"
#include <dns_sd.h>

#include <vector>
#include <string>
#include <thread>


typedef void (*domain_callback_def)(std::string);

struct domain_callback_store {
    domain_callback_def call;
    std::vector<std::string> *domain_list;
};

// private

void 
mdns::MDnsSub::DomainCallback_
(
    DNSServiceRef       sd_ref, 
    DNSServiceFlags     flags, 
    uint32_t            interface_index, 
    DNSServiceErrorType error_code, 
    const char          *reply_domain, 
    void                *context
) 
{
    struct domain_callback_store *store = (struct domain_callback_store *) context;
    store->domain_list->push_back(reply_domain);
    store->call(reply_domain);
}

void
mdns::MDnsSub::RequestStopDomain_() {
    if (this->domain_loop_ != NULL) { // if thread in runing, free it
        this->is_domain_loop_ = 0;
        this->domain_loop_->join();
        delete this->domain_loop_;
    }
}

void 
mdns::MDnsSub::ServiceCallback_
(
    DNSServiceRef       sd_ref, 
    DNSServiceFlags     flags, 
    uint32_t            interface_index, 
    DNSServiceErrorType error_code, 
    const char          *service_name, 
    const char          *regist_type, 
    const char          *reply_domain, 
    char                *context
) 
{

}

void 
mdns::MDnsSub::RecordCallback_
(
    DNSServiceRef       sd_ref,
    DNSServiceFlags     flags,
    uint32_t            interface_index,
    DNSServiceErrorType error_code,
    const char          *fullname,
    uint16_t            regist_record_type,
    uint16_t            regist_record_class,
    uint16_t            record_data_len,
    const void          *rdata,
    uint32_t            ttl,
    void                *context
)
{

}

// public

mdns::MDnsSub::MDnsSub
(
    std::string name, 
    std::string regist_type, 
    std::string domain, 
    uint32_t interface_index
) 
{
    this->name_ = name;
    this->regist_type_ = regist_type;
    this->domain_ = domain;
    this-> interface_index_ = interface_index;
}

mdns::MDnsSub::~MDnsSub() { 
    this->RequestStopDomain_();
}

int
mdns::MDnsSub::ScanDomain(void callback(std::string)) {
    this->domain_loop_ = new std::thread([&] {
        struct domain_callback_store store;
        store.call = callback;
        store.domain_list = &this->domain_list_;

        DNSServiceEnumerateDomains(
            &sd_ref_domain_, 
            kDNSServiceFlagsBrowseDomains, 
            0, 
            DomainCallback_, 
            &store
        );
        while (1) DNSServiceProcessResult(this->sd_ref_domain_);
    });
    return 0;
}

int
mdns::MDnsSub::ScanService(void callback()) {
    return 0;
}

int
mdns::MDnsSub::ScanRecord(void callback()) {
    return 0;
}

void
mdns::MDnsSub::Listen() {
    
}

// setter
void
mdns::MDnsSub::set_name(std::string name) {
    this->name_ = name;
}

void
mdns::MDnsSub::set_regist_type(std::string regist_type) {
    this->regist_type_ = regist_type;
}

void
mdns::MDnsSub::set_domain(std::string domain) {
    this->domain_ = domain;
}


void
mdns::MDnsSub::set_interface_index(uint32_t interface_index) {
    this->interface_index_ = interface_index;
}

// getter
std::string
mdns::MDnsSub::get_name() {
    return this->name_;
}

std::string
mdns::MDnsSub::get_regist_type() {
    return this->regist_type_;
}

std::string
mdns::MDnsSub::get_domain() {
    return this->domain_;
}

uint32_t
mdns::MDnsSub::get_interface_index() {
    return this->interface_index_;
}

long
mdns::MDnsSub::get_domain_list_size() {
    return this->domain_list_.size();
}

std::string
mdns::MDnsSub::get_domain_list_at(int i) {
    if (i < this->domain_list_.size()) {
        printf("??\n");
        return this->domain_list_.at(i);
    }
    throw "out of bound";
}
