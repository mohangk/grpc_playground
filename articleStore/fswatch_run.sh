#fswatch -e ".*\.o"  -e ".*\.pb.cc" -e ".*\.pb.h" -e article_client -e article_server .
fswatch -e ".*\.o"  -e ".*\.pb.cc" -e ".*\.pb.h" -e article_client -e article_server -o . | xargs -n1 -I{} ./autorun.sh
