"use client";

import { useEffect, type ReactNode } from "react";
import { useRouter } from "next/navigation";
import { useAuth } from "@/hooks/use-auth";
import Header from "@/components/header";
import { Skeleton } from "@/components/ui/skeleton";

export default function MainLayout({ children }: { children: ReactNode }) {
  const { user, loading } = useAuth();
  const router = useRouter();

  useEffect(() => {
    if (!loading && !user) {
      router.push("/login");
    }
  }, [user, loading, router]);

  if (loading || !user) {
    return (
      <div className="flex flex-col h-screen">
        <header className="flex items-center h-16 px-4 border-b shrink-0 md:px-6">
           <Skeleton className="h-8 w-32" />
           <div className="ml-auto">
             <Skeleton className="h-8 w-8 rounded-full" />
           </div>
        </header>
        <main className="flex flex-1 items-center justify-center">
          <Skeleton className="w-full max-w-md h-64" />
        </main>
      </div>
    );
  }

  return (
    <div className="flex min-h-screen w-full flex-col">
      <Header />
      {children}
    </div>
  );
}
