package me.com.grpc

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.LinearLayoutManager
import android.support.v7.widget.RecyclerView
import android.view.LayoutInflater
import android.view.ViewGroup
import com.jakewharton.rxbinding.view.RxView.clicks
import io.grpc.ManagedChannelBuilder
import io.grpc.examples.article.nano.Article
import io.grpc.examples.article.nano.ArticleStoreGrpc.newStub
import io.grpc.examples.article.nano.ArticlesForPeriodRequest
import io.grpc.stub.StreamObserver
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.item_article.view.*
import rx.Observable
import rx.android.schedulers.AndroidSchedulers
import rx.schedulers.Schedulers
import java.util.concurrent.TimeUnit

class MainActivity : AppCompatActivity() {

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_main)

    val adapter = ArticleAdapter()
    articleList.layoutManager = LinearLayoutManager(this)
    articleList.adapter = adapter

    clicks(btnHitServer)
      .throttleFirst(1, TimeUnit.SECONDS)
      .switchMap({
        requestWithNewConnection(addressInput.text.toString(), portInput.text.toString().toInt(),
          inputCategoryId.text.toString().toInt(), startTimestamp.text.toString(), endTimestamp.text.toString())
          .onBackpressureBuffer(10000)
          .map { it.id.toLong() to it.title }
          .onErrorReturn { RecyclerView.NO_ID to it.message }
          .scan(emptyList<Pair<Long, String>>(), { a, b -> a + b })
          .subscribeOn(Schedulers.newThread())
      })
      .subscribeOn(AndroidSchedulers.mainThread())
      .observeOn(AndroidSchedulers.mainThread())
      .subscribe({
        adapter.items = it
        adapter.notifyDataSetChanged()
      })
  }
}

fun requestWithNewConnection(ip: String, port: Int,
  categoryId: Int, startTimestamp: String, endTimestamp: String) = Observable.create<Article> { subscriber ->

  val req = ArticlesForPeriodRequest().apply {
    this.categoryId = categoryId
    this.startTimestamp = startTimestamp
    this.endTimestamp = endTimestamp
  }

  newStub(ManagedChannelBuilder.forAddress(ip, port).usePlaintext(true).build())
    .articlesForPeriod(req, object : StreamObserver<Article> {
      override fun onNext(value: Article) {
        value.let { subscriber.onNext(it) }
      }

      override fun onCompleted() {
        subscriber.onCompleted()
      }

      override fun onError(t: Throwable?) {
        subscriber.onError(t!!)
      }
    })
}

class ArticleAdapter() : RecyclerView.Adapter<RecyclerView.ViewHolder>() {
  var items: List<Pair<Long, String>> = emptyList()

  override fun getItemCount(): Int = items.count()

  override fun onBindViewHolder(holder: RecyclerView.ViewHolder?, position: Int) {
    holder?.itemView?.articleTitle?.text = items[position].second
  }

  override fun getItemId(position: Int): Long = items[position].first

  override fun onCreateViewHolder(parent: ViewGroup?, viewType: Int) =
    object : RecyclerView.ViewHolder(
      LayoutInflater.from(parent?.context).inflate(R.layout.item_article, parent, false)) {

    }
}